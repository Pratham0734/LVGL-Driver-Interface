#include <Wire.h>
#include <SPI.h>
#include <lvgl.h>
#include "ui.h"
#include "gfx_conf.h"

static lv_disp_draw_buf_t draw_buf;
static lv_color_t disp_draw_buf1[screenWidth * screenHeight / 8];
static lv_color_t disp_draw_buf2[screenWidth * screenHeight / 8];
static lv_disp_drv_t disp_drv;


/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
   uint32_t w = ( area->x2 - area->x1 + 1 );
   uint32_t h = ( area->y2 - area->y1 + 1 );

   tft.pushImageDMA(area->x1, area->y1, w, h,(lgfx::rgb565_t*)&color_p->full);

   lv_disp_flush_ready( disp );

}

void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
   uint16_t touchX, touchY;
   bool touched = tft.getTouch( &touchX, &touchY);
   if( !touched )
   {
      data->state = LV_INDEV_STATE_REL;
   }
   else
   {
      data->state = LV_INDEV_STATE_PR;

      /*Set the coordinates*/
      data->point.x = touchX;
      data->point.y = touchY;
   }
}


void setup()
{
  Serial.begin(9600);
  Serial.println("LVGL Widgets Demo");

  //GPIO init
#if defined (CrowPanel_50) || defined (CrowPanel_70)
  pinMode(38, OUTPUT);
  digitalWrite(38, LOW);
  pinMode(17, OUTPUT);
  digitalWrite(17, LOW);
  pinMode(18, OUTPUT);
  digitalWrite(18, LOW);
  pinMode(42, OUTPUT);
  digitalWrite(42, LOW);
#elif defined (CrowPanel_43)
  pinMode(20, OUTPUT);
  digitalWrite(20, LOW);
  pinMode(19, OUTPUT);
  digitalWrite(19, LOW);
  pinMode(35, OUTPUT);
  digitalWrite(35, LOW);
  pinMode(38, OUTPUT);
  digitalWrite(38, LOW);
  pinMode(0, OUTPUT);//TOUCH-CS
#endif

  //Display Prepare
  tft.begin();
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);
  delay(200);

  lv_init();

  delay(100);

  lv_disp_draw_buf_init(&draw_buf, disp_draw_buf1, disp_draw_buf2, screenWidth * screenHeight/8);
  /* Initialize the display */
  lv_disp_drv_init(&disp_drv);
  /* Change the following line to your display resolution */
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.full_refresh = 1;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  /* Initialize the (dummy) input device driver */
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);

  tft.fillScreen(TFT_BLACK);

  //please do not use LVGL Demo and UI export from Squareline Studio in the same time.
  // lv_demo_widgets();    // LVGL demo
  ui_init();
}
void loop()
{
    lv_timer_handler();
    delay(5);
}
