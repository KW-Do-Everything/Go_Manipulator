#include <memory>
#include <rclcpp/rclcpp.hpp>
#include <moveit/move_group_interface/move_group_interface.h>
#include <chrono>
#include <thread>

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  auto const node = std::make_shared<rclcpp::Node>(
    "hello_moveit",
    rclcpp::NodeOptions().automatically_declare_parameters_from_overrides(true)
  );

  auto const logger = rclcpp::get_logger("hello_moveit");

  // Create the MoveIt MoveGroup Interface for the arm
  using moveit::planning_interface::MoveGroupInterface;
  auto move_group_interface = MoveGroupInterface(node, "arm");

  auto const target_pose = []{
    geometry_msgs::msg::Pose msg;
    msg.orientation.x = 0.4005;
    msg.orientation.y = 0.5826;
    msg.orientation.z = 0.4005;
    msg.orientation.w = 0.58269;
    msg.position.x = -0.0669;
    msg.position.y = -0.568;
    msg.position.z = 0.1882;
    return msg;
  }();
  move_group_interface.setPoseTarget(target_pose);
  move_group_interface.setGoalPositionTolerance(0.1);
  move_group_interface.setGoalOrientationTolerance(0.1);

  auto const [success, plan] = [&move_group_interface]{
    moveit::planning_interface::MoveGroupInterface::Plan msg;
    auto const ok = static_cast<bool>(move_group_interface.plan(msg));
    return std::make_pair(ok, msg);
  }();

  if(success) {
    move_group_interface.execute(plan);
    std::this_thread::sleep_for(std::chrono::seconds(2));
  } else {
    RCLCPP_ERROR(logger, "Planning failed for the arm!");
  }

  auto gripper_interface = MoveGroupInterface(node, "gripper");

  gripper_interface.setNamedTarget("open");
  if (gripper_interface.move()) {
    RCLCPP_INFO(logger, "Gripper open successfully");
    std::this_thread::sleep_for(std::chrono::seconds(2));
  } else {
    RCLCPP_ERROR(logger, "Failed to set the gripper");
  }

  move_group_interface.setNamedTarget("home");
  if (move_group_interface.move()) {
    RCLCPP_INFO(logger, "Arm moved back to home position");
    std::this_thread::sleep_for(std::chrono::seconds(2));
    gripper_interface.setNamedTarget("open");
    if (gripper_interface.move()) {
      RCLCPP_INFO(logger, "Gripper opened successfully");
    } else {
      RCLCPP_ERROR(logger, "Failed to open the gripper");
    }

  } else {
    RCLCPP_ERROR(logger, "Failed to move the arm back to home position");
  }

  rclcpp::shutdown();
  return 0;
}
