#include <memory>

#include "geometry_msgs/msg/transform_stamped.hpp"
#include "rclcpp/rclcpp.hpp"
#include "tf2/LinearMath/Quaternion.h"
#include "tf2_ros/static_transform_broadcaster.h"
/**
 * @ref https://docs.ros.org/en/humble/Tutorials/Intermediate/Tf2/Writing-A-Tf2-Static-Broadcaster-Cpp.html
 */
class StaticFramePublisher : public rclcpp::Node
{
  public:
    explicit StaticFramePublisher(char *transformation[]) : Node("pubtf")
    {
        tf_static_broadcaster_ = std::make_shared<tf2_ros::StaticTransformBroadcaster>(this);

        // Publish static transforms once at startup
        this->make_transforms(transformation);
    }

  private:
    void make_transforms(char *transformation[])
    {
        geometry_msgs::msg::TransformStamped t;
        tf2::Quaternion q;
        if (strcmp(transformation[1], "sta"))
        {
            // static tf
            t.header.frame_id = "world_link";
            t.child_frame_id = "base_link";
            t.transform.translation.x = 1.0f;
            t.transform.translation.y = 1.0f;
            t.transform.translation.z = 1.0f;

            q.setValue(0.7071068, 0, 0);
            t.transform.rotation.x = q.x();
            t.transform.rotation.y = q.y();
            t.transform.rotation.z = q.z();
            t.transform.rotation.w = q.w();
        }
        else
        {
            // dynamic tf
            t.header.stamp = this->get_clock()->now();
            t.header.frame_id = "world_link";
            t.child_frame_id = "base_link";

            t.transform.translation.x = atof(transformation[2]);
            t.transform.translation.y = atof(transformation[3]);
            t.transform.translation.z = atof(transformation[4]);
            q.setRPY(atof(transformation[5]), atof(transformation[6]), atof(transformation[7]));
            t.transform.rotation.x = q.x();
            t.transform.rotation.y = q.y();
            t.transform.rotation.z = q.z();
            t.transform.rotation.w = q.w();
        }
        tf_static_broadcaster_->sendTransform(t);
    }

    std::shared_ptr<tf2_ros::StaticTransformBroadcaster> tf_static_broadcaster_;
};

int main(int argc, char *argv[])
{
    auto logger = rclcpp::get_logger("logger");

    // Obtain parameters from command line arguments
    if (argc != 8)
    {
        RCLCPP_INFO(logger, "Invalid number of parameters\nusage: "
                            "$ ros2 run tf_test pubtf "
                            "[sta|dyn] x y z roll pitch yaw");
        return 1;
    }

    // As the parent frame of the transform is `world`, it is
    // necessary to check that the frame name passed is different
    if (strcmp(argv[1], "world") == 0)
    {
        RCLCPP_INFO(logger, "Your static turtle name cannot be 'world'");
        return 1;
    }

    // Pass parameters and initialize node
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<StaticFramePublisher>(argv));
    rclcpp::shutdown();
    return 0;
}