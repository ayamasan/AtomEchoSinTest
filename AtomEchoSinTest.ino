// 1KHzのサイン波を再生

#include <driver/i2s.h>
#include <M5Atom.h>

//#include "wav.h"
const unsigned char wav[32] = {
0x00, 0x00,
0x97, 0x13,
0x33, 0x24,
0x4D, 0x2F,
0x32, 0x33,
0x4D, 0x2F,
0x33, 0x24,
0x97, 0x13,
0x00, 0x00,
0x68, 0xEC,
0xCC, 0xDB,
0xB2, 0xD0,
0xCD, 0xCC,
0xB2, 0xD0,
0xCC, 0xDB,
0x68, 0xEC
};

#define CONFIG_I2S_BCK_PIN      19
#define CONFIG_I2S_LRCK_PIN     33
#define CONFIG_I2S_DATA_PIN     22
#define CONFIG_I2S_DATA_IN_PIN  23

#define SPEAKER_I2S_NUMBER      I2S_NUM_0

#define MODE_MIC                0
#define MODE_SPK                1

unsigned char SONG[3200];  // 1sec


void InitI2SSpeakerOrMic(int mode)
{
	esp_err_t err = ESP_OK;
	
	i2s_driver_uninstall(SPEAKER_I2S_NUMBER);
	i2s_config_t i2s_config = {
		.mode                 = (i2s_mode_t)(I2S_MODE_MASTER),
		.sample_rate          = 16000,
		.bits_per_sample      = I2S_BITS_PER_SAMPLE_16BIT,
		.channel_format       = I2S_CHANNEL_FMT_ALL_RIGHT,
		.communication_format = I2S_COMM_FORMAT_I2S,
		.intr_alloc_flags     = ESP_INTR_FLAG_LEVEL1,
		.dma_buf_count        = 6,
		.dma_buf_len          = 60,
		.use_apll             = false,
		.tx_desc_auto_clear   = true,
		.fixed_mclk           = 0
	};
	
	if(mode == MODE_MIC){
		i2s_config.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_PDM);
	}
	else{
		i2s_config.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX);
	}
	
	err += i2s_driver_install(SPEAKER_I2S_NUMBER, &i2s_config, 0, NULL);
	
	i2s_pin_config_t tx_pin_config = {
		.bck_io_num           = CONFIG_I2S_BCK_PIN,
		.ws_io_num            = CONFIG_I2S_LRCK_PIN,
		.data_out_num         = CONFIG_I2S_DATA_PIN,
		.data_in_num          = CONFIG_I2S_DATA_IN_PIN,
	};
	
	err += i2s_set_pin(SPEAKER_I2S_NUMBER, &tx_pin_config);
	
	if(mode != MODE_MIC){
		err += i2s_set_clk(SPEAKER_I2S_NUMBER, 16000, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_MONO);
	}
	
	i2s_zero_dma_buffer(SPEAKER_I2S_NUMBER);
}


void setup() 
{
	// put your setup code here, to run once:
	M5.begin(true, false, true);
	delay(50);
	Serial.println();
	M5.dis.drawpix(0, CRGB(128, 128, 0));
	
	// sin波作成
	for(int i=0; i<100; i++){
		for(int j=0; j<32; j++){
			SONG[i*32+j] = wav[j];
		}
	}
	
	InitI2SSpeakerOrMic(MODE_MIC);
	delay(2000);
	
}


void loop() 
{
	// put your main code here, to run repeatedly:
	M5.update();
	
	if(M5.Btn.wasPressed()){
		size_t bytes_written;
		
		M5.dis.drawpix(0, CRGB(255, 0, 0));
		
		InitI2SSpeakerOrMic(MODE_SPK);
		
		// Write Speaker
		i2s_write(SPEAKER_I2S_NUMBER, SONG, 3200, &bytes_written, portMAX_DELAY);
		i2s_zero_dma_buffer(SPEAKER_I2S_NUMBER);
		
		// Set Mic Mode
		InitI2SSpeakerOrMic(MODE_MIC);
		
		delay(300);
		M5.dis.drawpix(0, CRGB(0, 0, 0));
	}
	delay(300);
}
