// Copyright 2024, Evan Palmer
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#pragma once

#include <string>

#include "geometry_msgs/msg/pose.hpp"
#include "geometry_msgs/msg/pose_stamped.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "geometry_msgs/msg/twist_stamped.hpp"
#include "geometry_msgs/msg/wrench.hpp"
#include "geometry_msgs/msg/wrench_stamped.hpp"
#include "message_transforms/transforms.hpp"
#include "nav_msgs/msg/odometry.hpp"
#include "rclcpp/qos.hpp"
#include "rclcpp_components/register_node_macro.hpp"
#include "rclcpp_lifecycle/lifecycle_node.hpp"

// auto-generated by generate_parameter_library
#include "message_transforms_parameters.hpp"

namespace m2m
{

using CallbackReturn = rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn;

class MessageTransforms : public rclcpp_lifecycle::LifecycleNode
{
public:
  explicit MessageTransforms(const rclcpp::NodeOptions & options = rclcpp::NodeOptions());

  auto on_configure(const rclcpp_lifecycle::State & state) -> CallbackReturn override;

private:
  /// Transform incoming messages and re-publish on a given topic.
  template <typename MessageT>
  auto register_transform(const std::string & in_topic, const std::string & out_topic, const rclcpp::QoS & qos) -> void
  {
    auto pub = create_publisher<MessageT>(out_topic, qos);
    subscribers_.push_back(create_subscription<MessageT>(in_topic, qos, [pub](const std::shared_ptr<MessageT> message) {
      MessageT m = *message;
      transforms::transform_message(m);
      pub->publish(m);
    }));
  }

  /// Transform and re-publish "Stamped" messages (e.g., PoseStamped, TwistStamped, etc.).
  template <typename MessageT>
  auto register_transform(
    const std::string & in_topic,
    const std::string & out_topic,
    const std::string & frame_id,
    const rclcpp::QoS & qos) -> void
  {
    auto pub = create_publisher<MessageT>(out_topic, qos);
    subscribers_.push_back(
      create_subscription<MessageT>(in_topic, qos, [pub, frame_id](const std::shared_ptr<MessageT> message) {
        MessageT m = *message;
        transforms::transform_message(m, frame_id);
        pub->publish(m);
      }));
  }

  /// Transform and re-publish incoming "Stamped" messages that require a child_frame_id (e.g., Odometry,
  /// TransformStamped, etc.).
  template <typename MessageT>
  auto register_transform(
    const std::string & in_topic,
    const std::string & out_topic,
    const std::string & frame_id,
    const std::string & child_frame_id,
    const rclcpp::QoS & qos) -> void
  {
    auto pub = create_publisher<MessageT>(out_topic, qos);
    subscribers_.push_back(create_subscription<MessageT>(
      in_topic, qos, [pub, frame_id, child_frame_id](const std::shared_ptr<MessageT> message) {
        MessageT m = *message;
        transforms::transform_message(m, frame_id, child_frame_id);
        pub->publish(m);
      }));
  }

  std::vector<std::shared_ptr<rclcpp::SubscriptionBase>> subscribers_;

  std::shared_ptr<message_transforms::ParamListener> param_listener_;
  message_transforms::Params params_;

  // Store transforms that do not require a frame_id or child_frame_id
  std::unordered_map<std::string, std::function<void(const std::string &, const std::string &, const rclcpp::QoS &)>>
    transform_map_ = {
      {"geometry_msgs/msg/Pose",
       [this](const std::string & in_topic, const std::string & out_topic, const rclcpp::QoS & qos) {
         register_transform<geometry_msgs::msg::Pose>(in_topic, out_topic, qos);
       }},
      {"geometry_msgs/msg/Twist",
       [this](const std::string & in_topic, const std::string & out_topic, const rclcpp::QoS & qos) {
         register_transform<geometry_msgs::msg::Twist>(in_topic, out_topic, qos);
       }},
      {"geometry_msgs/msg/Wrench",
       [this](const std::string & in_topic, const std::string & out_topic, const rclcpp::QoS & qos) {
         register_transform<geometry_msgs::msg::Wrench>(in_topic, out_topic, qos);
       }},
    };

  // Store transforms that require a frame_id
  std::unordered_map<
    std::string,
    std::function<void(const std::string &, const std::string &, const std::string &, const rclcpp::QoS &)>>
    transform_stamped_map_ = {
      {"geometry_msgs/msg/PoseStamped",
       [this](
         const std::string & in_topic,
         const std::string & out_topic,
         const std::string & frame_id,
         const rclcpp::QoS & qos) {
         register_transform<geometry_msgs::msg::PoseStamped>(in_topic, out_topic, frame_id, qos);
       }},
      {"geometry_msgs/msg/TwistStamped",
       [this](
         const std::string & in_topic,
         const std::string & out_topic,
         const std::string & frame_id,
         const rclcpp::QoS & qos) {
         register_transform<geometry_msgs::msg::TwistStamped>(in_topic, out_topic, frame_id, qos);
       }},
      {"geometry_msgs/msg/WrenchStamped",
       [this](
         const std::string & in_topic,
         const std::string & out_topic,
         const std::string & frame_id,
         const rclcpp::QoS & qos) {
         register_transform<geometry_msgs::msg::WrenchStamped>(in_topic, out_topic, frame_id, qos);
       }},
    };

  // Store transforms that require a frame_id and child_frame_id
  std::unordered_map<
    std::string,
    std::function<
      void(const std::string &, const std::string &, const std::string &, const std::string &, const rclcpp::QoS &)>>
    transform_odometry_map_ = {
      {"nav_msgs/msg/Odometry",
       [this](
         const std::string & in_topic,
         const std::string & out_topic,
         const std::string & frame_id,
         const std::string & child_frame_id,
         const rclcpp::QoS & qos) {
         register_transform<nav_msgs::msg::Odometry>(in_topic, out_topic, frame_id, child_frame_id, qos);
       }},
    };
};

}  // namespace m2m

RCLCPP_COMPONENTS_REGISTER_NODE(m2m::MessageTransforms)