from moveit_configs_utils import MoveItConfigsBuilder
from moveit_configs_utils.launches import generate_moveit_rviz_launch

def generate_launch_description():
    moveit_config = (
        MoveItConfigsBuilder("Go_manipulator", package_name="test2")
        .robot_description(file_path="/home/edward/Go_ws/src/Go_manipulator/Go_manipulator_description/urdf/Go_manipulator.urdf")  # URDF 파일 경로 설정
        .robot_description_semantic(file_path="/home/edward/Go_ws/src/Go_manipulator/test2/config/Go_manipulator.srdf")  # SRDF 파일 경로 설정
        .to_moveit_configs()
    )
    return generate_moveit_rviz_launch(moveit_config)
