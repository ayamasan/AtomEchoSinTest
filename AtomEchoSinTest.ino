// ボタン押しで1KHzのサイン波を再生

#include <driver/i2s.h>
#include <M5Atom.h>

// サイン波1波長分
const unsigned char wav[32] = {
0x7F, 0xFF,
0x98, 0x7C,
0xAD, 0x3F,
0xBB, 0x1F,
0xBF, 0xFE,
0xBB, 0x1F,
0xAD, 0x3F,
0x98, 0x7C,
0x7F, 0xFF,
0x67, 0x81,
0x52, 0xBE,
0x44, 0xDE,
0x3F, 0xFF,
0x44, 0xDE,
0x52, 0xBE,
0x67, 0x81
};

#define CONFIG_I2S_BCK_PIN      19
#define CONFIG_I2S_LRCK_PIN     33
#define CONFIG_I2S_DATA_PIN     22
#define CONFIG_I2S_DATA_IN_PIN  23

#define SPEAKER_I2S_NUMBER      I2S_NUM_0

#define MODE_MIC                0
#define MODE_SPK                1

unsigned char SONG[3200];  // 1sec


// マイク＆スピーカー設定
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
　　　　.bck_io_num    = CONFIG_I2S_BCK_PIN,
　　　　.ws_io_num     = CONFIG_I2S_LRCK_PIN,
　　　　.data_out_num  = CONFIG_I2S_DATA_PIN,
　　　　.data_in_num   = CONFIG_I2S_DATA_IN_PIN,
　　};
　　
　　err += i2s_set_pin(SPEAKER_I2S_NUMBER, &tx_pin_config);
　　
　　if(mode != MODE_MIC){
　　　　err += i2s_set_clk(SPEAKER_I2S_NUMBER, 16000, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_MONO);
　　}
　　
　　i2s_zero_dma_buffer(SPEAKER_I2S_NUMBER);
}

// 初期設定
void setup() 
{
　　// put your setup code here, to run once:
　　M5.begin(true, false, true);
　　delay(50);
　　Serial.println();
　　M5.dis.drawpix(0, CRGB(128, 128, 0));
　　
　　// sin波作成（1秒分）
　　for(int i=0; i<100; i++){
　　　　for(int j=0; j<32; j++){
　　　　　　SONG[i*32+j] = wav[j];
　　　　}
　　}
　　
　　InitI2SSpeakerOrMic(MODE_MIC);
　　delay(2000);
}


// 繰り返しメイン処理
void loop() 
{
　　// put your main code here, to run repeatedly:
　　M5.update();
　　
　　if(M5.Btn.wasPressed()){
　　　　size_t bytes_written;
　　　　
　　　　M5.dis.drawpix(0, CRGB(255, 0, 0));
　　　　
　　　　// スピーカーモード
　　　　InitI2SSpeakerOrMic(MODE_SPK);
　　　　
　　　　// スピーカーへデータ書き込み
　　　　i2s_write(SPEAKER_I2S_NUMBER, SONG, 3200, &bytes_written, portMAX_DELAY);
　　　　i2s_zero_dma_buffer(SPEAKER_I2S_NUMBER);
　　　　
　　　　// マイクモード
　　　　InitI2SSpeakerOrMic(MODE_MIC);
　　　　
　　　　delay(300);
　　　　M5.dis.drawpix(0, CRGB(0, 0, 0));
　　}
　　delay(300);
}
