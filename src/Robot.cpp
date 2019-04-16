#include<Robot.h>
#include<math.h>

Robot:: Robot(int n_joints,Eigen::MatrixXd m1, float j_min_limits[], float j_max_limits[])
{
    no_of_joints=n_joints;
    Dh_params.resize(no_of_joints,4);\
    HT_BasetoEF.resize(4,4);
    Dh_params=m1;
    joint_min_limits=new float[n_joints];
    joint_max_limits=new float[n_joints];
    for(int i=0;i<n_joints;i++)
    {
        joint_min_limits[i]=j_min_limits[i];
        joint_max_limits[i]=j_max_limits[i];
    }

}

std::vector<float> Robot::Forward_Kinematics(float joint_angles[])
{
    //Eigen::MatrixXd Base_to_EFF(4,4);
    for (int i=0;i<no_of_joints;i++)
    {
        Eigen::MatrixXd T(4,4);
        T(0,0)=cos(joint_angles[i]);
        T(0,1)=-sin(joint_angles[i])*cos(Dh_params(i,0));
        T(0,2)=sin(joint_angles[i])*sin(Dh_params(i,0));
        T(0,3)=Dh_params(i,1)*cos(joint_angles[i]);
        T(1,0)=sin(joint_angles[i]);
        T(1,1)=cos(joint_angles[i])*cos(Dh_params(i,0));
        T(1,2)=-cos(joint_angles[i])*sin(Dh_params(i,0));
        T(1,3)=Dh_params(i,1)*sin(joint_angles[i]);
        T(2,0)=0;
        T(2,1)=sin(Dh_params(i,0));
        T(2,2)=cos(Dh_params(i,0));
        T(2,3)=Dh_params(i,2);
        T(3,0)=0;
        T(3,1)=0;
        T(3,2)=0;
        T(3,3)=1;

        if(i==0)
        {
            HT_BasetoEF=T;
        }
        else
        {
            HT_BasetoEF=HT_BasetoEF*T;
        }
       // std::cout<<HT_BasetoEF<<std::endl;
    }

}

Robot::~Robot()
{
    delete[] joint_max_limits;
    delete[] joint_min_limits;
}

void Robot::Inverse_kinematics(float cartesian_positions[])
{
    float theta1,theta3[2],theta2[4],D;
    float a1=Dh_params(0,1);
    float a2=Dh_params(1,1);
    float a3=Dh_params(2,1);
    float alpha1=Dh_params(0,0);
    float alpha2=Dh_params(1,0);
    float alpha3=Dh_params(2,0);
    float x=cartesian_positions[0];
    float y=cartesian_positions[1];
    float z=cartesian_positions[2];
    //std::vector<std::vector<float> > All_IK_solution;
    std::vector<float> IK_solution;
    std::vector<float> closest_solution;
    theta1=atan2(y,x);
    D=(pow(x-a1*cos(theta1),2)+pow(y-a1*sin(theta1),2)+pow(z,2)-pow(a2,2)-pow(a3,2))/(2*a2*a3);
    theta3[0]=atan2(sqrt(1-pow(D,2)),D);
    theta3[1]=atan2(-sqrt(1-pow(D,2)),D);
    theta2[0]=atan2(z,sqrt(pow(x-a1*cos(theta1),2)+pow(y-a1*sin(theta1),2)))-atan2(a3*sin(theta3[0]),a2+a3*cos(theta3[0]));
    theta2[1]=atan2(z,-sqrt(pow(x-a1*cos(theta1),2)+pow(y-a1*sin(theta1),2)))-atan2(a3*sin(theta3[0]),a2+a3*cos(theta3[0]));
    theta2[2]=atan2(z,sqrt(pow(x-a1*cos(theta1),2)+pow(y-a1*sin(theta1),2)))-atan2(a3*sin(theta3[1]),a2+a3*cos(theta3[1]));
    theta2[3]=atan2(z,-sqrt(pow(x-a1*cos(theta1),2)+pow(y-a1*sin(theta1),2)))-atan2(a3*sin(theta3[1]),a2+a3*cos(theta3[1]));


    IK_solutions.clear();
    IK_solution.push_back(fmod(theta1,2*M_PI));
    IK_solution.push_back(fmod(theta2[0],2*M_PI));
    IK_solution.push_back(fmod(theta3[0],2*M_PI));
    IK_solutions.push_back(IK_solution);
    IK_solution.clear();

    IK_solution.push_back(fmod(theta1,2*M_PI));
    IK_solution.push_back(fmod(theta2[1],2*M_PI));
    IK_solution.push_back(fmod(theta3[0],2*M_PI));
    IK_solutions.push_back(IK_solution);
    IK_solution.clear();

    IK_solution.push_back(fmod(theta1,2*M_PI));
    IK_solution.push_back(fmod(theta2[2],2*M_PI));
    IK_solution.push_back(fmod(theta3[1],2*M_PI));
    IK_solutions.push_back(IK_solution);
    IK_solution.clear();

    IK_solution.push_back(fmod(theta1,2*M_PI));
    IK_solution.push_back(fmod(theta2[3],2*M_PI));
    IK_solution.push_back(fmod(theta3[1],2*M_PI));
    IK_solutions.push_back(IK_solution);
    IK_solution.clear();

    validate_IK_solutions();

    //float current_position[]={0,0,0};
    //closest_solution=closest_IK_solutions(current_position);
//    std::cout<<"Solution:"<<std::endl<<closest_solution[0]<<std::endl<<closest_solution[1]<<std::endl<<closest_solution[2];

}

void Robot:: validate_IK_solutions()
{
    valid_ik_solutions.clear();
    for (int i=0;i<IK_solutions.size();i++)
    {
        if (IK_solutions[i][0]>=joint_min_limits[0] and IK_solutions[i][1]>=joint_min_limits[1] and IK_solutions[i][2]>=joint_min_limits[2] and IK_solutions[i][0]<=joint_max_limits[0] and IK_solutions[i][1]<=joint_max_limits[1] and IK_solutions[i][2]<=joint_max_limits[2])
            valid_ik_solutions.push_back( IK_solutions[i]);
//            std::cout<<valid_ik_solutions[i][0];
//            std::cout<<valid_ik_solutions[i][1];
//            std::cout<<valid_ik_solutions[i][2];
      }
}

std::vector<float> Robot::closest_IK_solutions(float current_position[],float goal_position[]) {
    Inverse_kinematics(goal_position);
    float distance[valid_ik_solutions.size()];
    for (int i=0;i<valid_ik_solutions.size();i++)
    {
      distance[i]=sqrt(pow(valid_ik_solutions[i][0]-current_position[0],2)+ pow(valid_ik_solutions[i][1]-current_position[1],2)+pow(valid_ik_solutions[i][2]-current_position[2],2));
      //std::cout<<"distance="<<distance[i]<<std::endl;
    }
    float Min=distance[0];
    int index=0;
    for(int i=1;i<valid_ik_solutions.size();i++)
    {
        if (distance[i] < Min) {
            Min = distance[i];
            index = i;
        }
    }
    //std::cout<<"Closest_point"<<std::endl<<valid_ik_solutions[index][0]<<std::endl<<valid_ik_solutions[index][1]<<std::endl<<valid_ik_solutions[index][2];
    return valid_ik_solutions[index];
}
void Robot::print() {
   // for (int i=0;i<no_of_joints;i++)
//    {
//        for (int j=0;j<4;j++)
//            std::cout<<Dh_params(i,j)<<std::endl;
//    }

    for (int i=0;i<valid_ik_solutions.size();i++) {

        std::cout << "theta1=" << valid_ik_solutions[i][0] << std::endl;
        std::cout << "theta2=" << valid_ik_solutions[i][1] << std::endl;
        std::cout << "theta3=" << valid_ik_solutions[i][2] << std::endl;

    }
}
