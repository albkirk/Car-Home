//  - - - libraries
#include <math.h>

//  - - -  Constants
#define SPEAKER_PIN 25


//  - - -  Variables  - - -
byte last_Volume = 0;
byte min_ampl=9;        // Max is 8 bits - 1 Byte
byte max_ampl=245;      // Max is 8 bits - 1 Byte
bool player_stop = false;

// Variables used for multi-task
TaskHandle_t task_DAC;
TaskHandle_t task_PlayMEM;
TaskHandle_t task_PlayFile;
size_t task_idx = 0;
#define buf_size 16384
struct task_parameters {
    const uint8_t *DAC_data;
    uint32_t sample_rate;
    uint8_t repeat;
    const char *path;
} static task_param;

struct __attribute__((__packed__)) buffer_struct {
    uint8_t data[buf_size];
    bool Buf_read_done;
    bool Buf_write_done;
};

static buffer_struct Buffer[2];
bool DATA_done=true;
byte play_status = 0;
/*
    0 -> IDLE
    1 -> Play finish
    2 -> Play Busy
    3 -> DAC finish
    4 -> DAC Busy
*/


//  - - -  Functions  - - -
// Setup the Timed interrupt
void Touch_to_Stop() {
  if (TL_COUNT>=1) {      // Was Touch Button pressed?
    TL_COUNT = 0;
    //TL_STATUS = false;
    player_stop = true;
  }
}

void player_mute() {
    dacWrite(SPEAKER_PIN, 127);
}

void player_fadein(byte amplitude = 127, uint32_t sample_rate = 22050) {
    unsigned int delay_interval = ((unsigned long)1000000/sample_rate);
    if (amplitude > 127) {
        for(byte fadein=127; fadein < amplitude; fadein++) {
            dacWrite(SPEAKER_PIN, fadein);
            delayMicroseconds(delay_interval);
        }
    }
    else {
        for(byte fadein=127; fadein > amplitude; fadein--) {
            dacWrite(SPEAKER_PIN, fadein);
            delayMicroseconds(delay_interval);
        }

    }
}

void player_fadeout(byte amplitude = 127, uint32_t sample_rate = 22050) {
    unsigned int delay_interval = ((unsigned long)1000000/sample_rate);
    if (amplitude > 127) {
        for(byte fadeout=amplitude; fadeout>=127; fadeout--) {
            dacWrite(SPEAKER_PIN, fadeout);
            delayMicroseconds(delay_interval);
        }
    }
    else {
        for(byte fadeout=amplitude; fadeout<=127; fadeout++) {
            dacWrite(SPEAKER_PIN, fadeout);
            delayMicroseconds(delay_interval);
        }

    }
}


void player_tone(float freq, float tempo, byte ampl=config.Volume, byte bpm=120, uint32_t spl_rate = 22050) {
    TL_COUNT = 0;
    ampl = (byte)ampl/100.0*(max_ampl - min_ampl)/2;     // To convert volume [0-100%] to amplitude [0-128]
    if (ampl > 128) ampl = 128;
    if (ampl < 0) ampl = 0;
    uint32_t delay_intv = ((uint32_t)1000000/spl_rate);
    uint32_t Num_of_samples = (tempo * 60 / bpm * spl_rate);
    float sin_const = 2*PI*freq*delay_intv*0.000001;
    uint32_t n = 0;
    byte sample = 0;
    player_fadein((byte)((ampl * sin(2*PI*freq*n*delay_intv*0.000001))+127), spl_rate);
    for (n = 0; n < Num_of_samples; n++) {
        sample = (byte)( ampl * sin(sin_const*n)) + 127;
        dacWrite(SPEAKER_PIN, sample);
        delayMicroseconds(delay_intv-16);     // 16 --> to compensate the time used to calculate the "sample" value
    }
    player_fadeout((byte)((ampl * sin(2*PI*freq*n*delay_intv*0.000001))+127), spl_rate);
    player_mute();
}

void player_beepup(int ntimes = 2) {
    player_stop = false;
    for (size_t i = 0; i < ntimes; i++) {
        player_tone(A6, 1.0/16);
        player_tone(0, 1.0/16);
        player_tone(B6, 1.0/8);
        player_tone(0, 1.0/2);
        if (player_stop) i = ntimes;
    };
}

void player_beepdn(int ntimes = 2) {
    player_stop = false;
    for (size_t i = 0; i < ntimes; i++) {
        player_tone(A6, 1.0/8);
        player_tone(0, 1.0/16);
        player_tone(G6, 1.0/16);
        player_tone(0, 1.0/2);
        if (player_stop) i = ntimes;
    };
}

void player_beep(int ntimes = 2) {
    player_stop = false;
    for (size_t i = 0; i < ntimes; i++) {
        player_tone(A6*2, 1.0/8);
        player_tone(0, 1.0/8);
        player_tone(0, 1.0/4);
        if (player_stop) i = ntimes;
    };
}


void fn_task_DAC(void *parameters){
  uint8_t buf_idx = 0;
  //Serial.println("fn_task_DAC running on core " + String(xPortGetCoreID()));
  play_status = 4;
  telnet_println("play_status: " + String(play_status));
  uint32_t delay_intv = ((uint32_t)1000000/task_param.sample_rate);
  uint32_t Num_of_samples;
  do {
      while (!Buffer[buf_idx].Buf_write_done && !DATA_done) {
          telnet_println("NO Buffer Write DONE!");
          //delayMicroseconds(delay_intv);
          delay(1000);
        };
      task_param.DAC_data = Buffer[buf_idx].data;
      Num_of_samples = strlen((char*)task_param.DAC_data)-1;
      //telnet_println("buf_idx: " + String(buf_idx) + "  Num_of_samples: " + String(Num_of_samples));
      for(task_idx=0; task_idx < Num_of_samples; task_idx++) {
          if (player_stop) {
              telnet_println("player_stop on: fn_task_DAC");
              Buffer[buf_idx].Buf_read_done = true;
              goto stop_DAC;
          };
          dacWrite(SPEAKER_PIN, (byte)((task_param.DAC_data[task_idx]-127)*(config.Volume/100.0)+127));
          delayMicroseconds(delay_intv-11);   // use -12
      };
      Buffer[buf_idx].Buf_write_done=false;
      Buffer[buf_idx].Buf_read_done = true;
      buf_idx=(buf_idx+1)%2;
  } while (!(DATA_done && Buffer[0].Buf_read_done && Buffer[1].Buf_read_done));
  stop_DAC:;
  play_status = 3;
  telnet_println("play_status: " + String(play_status));
  vTaskDelete(NULL);
}

void fn_task_PlayMEM(void *parameters) {
  Serial.println("fn_task_PlayMEM running on core " + String(xPortGetCoreID()));
  play_status = 4;        // It should be 2, but this little hack "4" is required to run the WHILE loop
  //telnet_println("play_status: " + String(play_status));
  player_fadein((byte)((task_param.DAC_data[0]-127)*(config.Volume/100.0)+127), task_param.sample_rate);
  for (size_t n = 0; n < task_param.repeat + 1; n++) {
      xTaskCreate(fn_task_DAC, "task_DAC", 8192, (void *)NULL, 0, &task_DAC);
      while (play_status == 4) delay(25);
      if (player_stop) {
          goto stop_PlayMEM;
      };
  };
  stop_PlayMEM:;
  player_fadeout((byte)((task_param.DAC_data[task_idx]-127)*(config.Volume/100.0)+127), task_param.sample_rate);
  player_mute();
  play_status = 1;
  telnet_println("play_status: " + String(play_status));
  vTaskDelete(NULL);
}

void player_music(const uint8_t *music_data, byte rept=0, uint32_t spl_rate=22050) {
    Serial.println("player_music running on core " + String(xPortGetCoreID()));
    TL_COUNT = 0;
    player_stop = false;
    if(config.Volume != 0) {
        task_param.DAC_data = music_data;
        task_param.sample_rate=spl_rate;
        task_param.repeat=rept;
        xTaskCreate(fn_task_PlayMEM, "task_PlayMEM", 8192, (void *)NULL, 0, &task_PlayMEM);
    };
}

void fn_task_PlayFile(void *parameters) {
    uint8_t buf_idx;
    uint32_t Num_of_samples;        //len = file.size();
    uint32_t toRead;
    byte sample;
    //Serial.println("fn_task_PlayFile running on core " + String(xPortGetCoreID()));
    for (size_t n = 0; n < task_param.repeat + 1; n++) {
        telnet_println("Repeating for the " + String(n+1) + " time.");
        buf_idx = 0;
        Buffer[0].Buf_read_done=true;
        Buffer[1].Buf_read_done=true;

        Serial.printf("Reading file: %s\n", task_param.path);
        File file = SD.open(task_param.path);
        if(!file){
            Serial.println("Failed to open file for reading");
        }
        else {
            play_status = 2;
            DATA_done=false;
            telnet_println("play_status: " + String(play_status));
            Num_of_samples = file.size();
            Serial.println("Music file lenght: " + String(Num_of_samples));
            sample = file.read();
            player_fadein((byte)((sample-127)*(config.Volume/100.0)+127), task_param.sample_rate);
            //telnet_println("FadeIn Done with sample: " + (String)sample);
            while(Num_of_samples > 0){
                //telnet_println("Num_of_samples: " + String(Num_of_samples));
                toRead = Num_of_samples;
                if(toRead >= buf_size){
                    toRead = buf_size-1;
                }
                do {
                    if (Buffer[buf_idx].Buf_read_done) {
                        //telnet_println("writing on Buffer: " + String(buf_idx));
                        file.read(Buffer[buf_idx].data, toRead);
                        Buffer[buf_idx].data[toRead]=0;
                        Buffer[buf_idx].Buf_read_done=false;
                        Buffer[buf_idx].Buf_write_done=true;
                        buf_idx = (buf_idx+1)%2;
                    }
                    else {
                        //telnet_println("NO Buffer read DONE!");
                        delay(150);
                    };
                    if (player_stop && play_status==3) {
                        telnet_println("player_stop on: fn_task_PlayFile");
                        n = task_param.repeat;
                        goto stop_PlayFile;
                    };
                } while (!Buffer[buf_idx].Buf_read_done);
                if (play_status == 2) xTaskCreatePinnedToCore(fn_task_DAC, "task_DAC", 8192, (void *)NULL, 0, &task_DAC, 0);
                Num_of_samples -= toRead;
                delay(50);
            };
        };
        stop_PlayFile:;
        DATA_done=true;
        //telnet_println("DATA_done: " + String(DATA_done));
        while (play_status == 4) delay(1);
        player_fadeout((byte)((task_param.DAC_data[task_idx]-127)*(config.Volume/100.0)+127), task_param.sample_rate);
        player_mute();
        file.close();
        //delay(100);
    }
    play_status = 1;
    telnet_println("play_status: " + String(play_status));
    vTaskDelete(NULL);
}

void player_playfile(fs::FS &fs, const char * path, byte rept=0, uint32_t spl_rate=22050) {
  //Serial.println("player_music running on core " + String(xPortGetCoreID()));
  TL_COUNT = 0;
  player_stop = false;
  if(config.Volume != 0 && play_status <= 1) {
      task_param.sample_rate=spl_rate;
      task_param.repeat=rept;
      task_param.path=path;
      //xTaskCreate(fn_task_PlayFile, "task_PlayFile", 8192, (void *)NULL, 0, &task_PlayFile);
      xTaskCreatePinnedToCore(fn_task_PlayFile, "task_PlayFile", 8192, (void *)NULL, 1, &task_PlayFile, 1);
  }
}


void player_play (byte idx = 0) {
  // sounds[] = {"Birds", "Ocean", "Crickets", "music box", "Waterfall", "Startup", "WakeUp", "Cucu"};
  switch (idx) {
        case 0 :
            player_playfile(SD, "/Nightingale.raw", 3);
            break;
        case 1 :
            player_playfile(SD, "/Ocean.raw", 3);
            break;
        case 2 :
            player_playfile(SD, "/Cricket.raw", 5);
            break;
        case 3 :
            //player_music(MusicBox, 3);
            player_playfile(SD, "/Music-box.raw", 3);
            break;
        case 4 :
            player_beepdn(60);
            break;
        case 5 :
            player_playfile(SD, "/1KHz_30sec.raw");
            //player_music(startup_sound);
            break;
        case 6 :
            player_beepup(60);
            break;
        case 7 :
            player_playfile(SD, "/Cuckoo.raw", 5);
            //player_music(Cuckoo, 5);
            break;
        default :
            player_beep(6);
  }
}

void player_setup() {
    player_mute();
}


void player_loop() {
    Touch_to_Stop();
}
