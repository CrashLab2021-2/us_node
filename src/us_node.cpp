#include <stdio.h>
#include <unistd.h>

#include <pigpiod_if2.h>
#include <ros/ros.h>
#include "std_msgs/Float32.h"

// #define TRIG_PINNO 27
// #define ECHO_PINNO 17
int TRIG_PINNO;
int ECHO_PINNO;


float distance;
int pi;

void cb_func_echo(int pi, unsigned gpio, unsigned level, uint32_t tick);

int us_setting(){
    if ((pi = pigpio_start(NULL, NULL)) < 0) return 1;
    
    set_mode(pi, TRIG_PINNO, PI_OUTPUT);
    set_mode(pi, ECHO_PINNO, PI_INPUT);
    callback(pi, ECHO_PINNO, EITHER_EDGE, cb_func_echo);
    gpio_write(pi, TRIG_PINNO, PI_OFF);
    time_sleep(1);     // delay 1 second
}

uint32_t start_tick_, dist_tick_;

int main(int argc, char *argv[])
{
    ros::init(argc, argv, "us_node");
    ros::NodeHandle nh;
    
    ECHO_PINNO = atoi(argv[1]);
    TRIG_PINNO = atoi(argv[2]);
    
    //nh.getParam("/trig",TRIG_PINNO);
    //nh.getParam("/echo",ECHO_PINNO);
    ROS_INFO("TRIG %d", TRIG_PINNO);
    ROS_INFO("ECHO %d", ECHO_PINNO);
    
    ros::Publisher us_pub = nh.advertise<std_msgs::Float32>("us_topic", 1);
    
    std_msgs::Float32 us_msg;
    
    us_setting();
    printf("Raspberry Pi HC-SR04 UltraSonic sensor\n" );

    while(ros::ok()){
        start_tick_ = dist_tick_ = 0;
        gpio_trigger(pi, TRIG_PINNO, 10, PI_HIGH);
        time_sleep(0.05);

        if(dist_tick_ && start_tick_){
            //distance = (float)(dist_tick_) / 58.8235;
            distance = dist_tick_ / 1000000. * 340 / 2 * 100; //m -> cm
            if(distance < 2 || distance > 400)
                printf("range error\n");
            else
                printf("interval : %6dus, Distance : %6.1f cm\n", dist_tick_, distance);
            us_msg.data = distance;
        }
        else{
            printf("sense error\n");
            us_msg.data = -1;
        }
        us_pub.publish(us_msg);

        time_sleep(0.1);
    }
    pigpio_stop(pi);

    return 0;
}

void cb_func_echo(int pi, unsigned gpio, unsigned level, uint32_t tick)
{
    if(level == PI_HIGH)
        start_tick_ = tick;
    else if(level == PI_LOW)
        dist_tick_ = tick - start_tick_;
}
