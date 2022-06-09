#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <opencv2/opencv.hpp>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

static const std::string OPENCV_STANDARD_IMAGE = "Standard Image";
static const std::string OPENCV_CROPPED_IMAGE = "Cropped Image";

class ImageCropper
{
  ros::NodeHandle nh_;
  image_transport::ImageTransport it_;
  image_transport::Subscriber image_sub_;
  image_transport::Publisher image_pub_;
  sensor_msgs::ImagePtr msg_;

public:
  ImageCropper()
    : it_(nh_)
  {
    // Subscrive to input video feed and publish output video feed
    image_sub_ = it_.subscribe("/camera1/image_raw", 1,
      &ImageCropper::imageCallback, this);
    image_pub_ = it_.advertise("/camera1/image_cropped", 1);

    cv::namedWindow(OPENCV_STANDARD_IMAGE);
    cv::namedWindow(OPENCV_CROPPED_IMAGE);
  }

  ~ImageCropper()
  {
    cv::destroyWindow(OPENCV_STANDARD_IMAGE);
    cv::destroyWindow(OPENCV_CROPPED_IMAGE);
  }

  void imageCallback(const sensor_msgs::ImageConstPtr& msg)
  {
    cv_bridge::CvImagePtr cv_ptr;
    try
    {
      cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
    }
    catch (cv_bridge::Exception& e)
    {
      ROS_ERROR("cv_bridge exception: %s", e.what());
      return;
    }

    int image_width = cv_ptr->image.cols;
    int image_height = cv_ptr->image.rows;

    // Parameters for cropping
    cv::Rect crop_region(0, 0, image_width/2, image_height/2);
    cv::Mat image_cropped = cv_ptr->image(crop_region);

    // ROS_INFO("Dimensions of image: %d x %d", image_height, image_width);    // DEBUG

    // Update GUI Window
    cv::imshow(OPENCV_STANDARD_IMAGE, cv_ptr->image);
    cv::imshow(OPENCV_CROPPED_IMAGE, image_cropped);
    cv::waitKey(300);   // Shows image updates every 300 ms
    
    // Output modified video stream
    msg_ = cv_bridge::CvImage(std_msgs::Header(), sensor_msgs::image_encodings::BGR8, image_cropped).toImageMsg();
    image_pub_.publish(msg_);
  }
};

int main(int argc, char** argv)
{
  ros::init(argc, argv, "image_converter");
  ImageCropper ic;
  ros::spin();
  return 0;
}