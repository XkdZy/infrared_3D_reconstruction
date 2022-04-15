/****************************************************************************
*----------------------------------------------------------------------------*
*  projectname   : 3D reconstruction                                         *
*----------------------------------------------------------------------------*
*  Change History :                                                          *
*  <Date>     | <Version> | <Author>      | <Description>                    *
*----------------------------------------------------------------------------*
*  2021/11/15 | 1.0.0     |  liulei       | 3D reconstruction base on Open3D *
*----------------------------------------------------------------------------*
*****************************************************************************/
#include "Reconstruction.h"
#include "SerialPort.h"
#include "MagCamera.h"
#include "AstraCameraD2C.h"
#include "CalibrateEyeInHand.h"
#include "AdaptiveThreshold.h"

using namespace std;
using namespace cv;

#define ISHAVE_SERIAL_ROBOT     0   //�Ƿ��л�е�۴���
#define ISHAVE_SERIAL_LASER     0   //�Ƿ��м��⴮��
#define ISHAVE_THERMALCAMERA    0   //�Ƿ���������
#define ISHAVE_ASTRACAMERA      0   //�Ƿ���astra������


//���ݵ�Ŀ¼
string dataPath = "F:\\00reconstrcuction\\04data\\20220413data";

Mat_<double> CameraMatrix = (Mat_<double>(3, 4) <<
    516.2864346444153, 0, 312.7447369845078, 0,
    0, 516.1200849732774, 238.1377658233807, 0,
    0, 0, 1, 0);
Mat_<double> ThermalMatrix = (Mat_<double>(3, 4) <<
    361.8141867468519, 0, 328.9660615139343, 0,
    0, 361.7977913724541, 244.5513891430242, 0,
    0, 0, 1, 0);
Mat_<double> H_Thermal2Gripper = (Mat_<double>(4, 4) <<
    0.9995488327094058, -0.01793564014333856, 0.02409240216153416, 2.190745313048881,
    0.01742323320309355, 0.9996210311299785, 0.02131255656624471, -0.3131241491923973,
    -0.02446552623621761, -0.02088317349655996, 0.9994825316586067, 184.121955439739,
    0, 0, 0, 1);
Mat_<double> H_Camera2Gripper = (Mat_<double>(4, 4) <<
    0.01826269947513948, 0.9989752991892082,  -0.04141045058572479, -284.8733202498663,
    -0.9970075388753361, 0.01508382704278766, -0.07581850425515202, 9.114235632684117,
    -0.07511618489796977,0.04267118198005862, 0.9962613758421023,   26.40496535591366,
    0,                   0,                   0,                    1);
Mat H_Gripper2Thermal;

//��е�۲ɼ�����ʱ����̬
vector<string> robotPositionCamera = {
    "A-5597-5174+2107+180+020+163BC",
    "A-2471-3034+2541-172+000+164BC",
    "A+0833-5740+3958-172-015+162BC",
};
vector<string> robotPositionThermal = {
    "A-3990-5244+0426-161+048+172BC",
    "A-4521-6902+0643-170+048+172BC",
    "A+0126-6379+2382-172+001+174BC",
    "A-0026-8203+2517-175+001+174BC",
    "A+3386-4838+0880-160-036+178BC",
    "A+3541-7589+1562-172-031-177BC",
};

//��������ָ��cv::Mat
std::shared_ptr<geometry::PointCloud> pcl_ptr = std::make_shared<geometry::PointCloud>();

//�����е��λ��
cv::Mat ToolPoseCamera(robotPositionCamera.size(), 6, CV_64F, 0.0);
cv::Mat ToolPoseThermal(robotPositionThermal.size(), 6, CV_64F, 0.0);

//����任����
vector<cv::Mat>  H_Gripper2Base_cameraVec;
vector<cv::Mat>  H_Base2Gripper_cameraVec;
vector<cv::Mat> H_Base2Gripper_thermalVec;

vector<Eigen::Vector3d> thermalPositionVec;
vector<Eigen::Vector3d> cameraPositionVec;
vector<cv::Mat> thermalImageVec;
vector<cv::Mat> depthImageVec;
vector<cv::Mat> colorImageVec;


struct Image {
    Image(int _imageIndex, int _u, int _v, int _distance) {
        imageIndex = _imageIndex;
        distance = _distance;
        u = _u;
        v = _v;
    }
    int imageIndex;
    int distance;
    int u;
    int v;
};

void GetColorFromImageVec(Eigen::Vector3d& colorTmp, const cv::Mat& p_base, string thermalOrCamera = "thermal");

int main(int argc, char* argv[]) {
    AstraCameraD2C* astraCameraD2C = new AstraCameraD2C;	//����������ʵ��������
#if ISHAVE_ASTRACAMERA
	if (astraCameraD2C->CameraInit(HARDWARE_D2C) != CAMERA_STATUS_SUCCESS) {
        cout << "Astra��������ʧ�ܣ�" << endl;
		return 2;
	}
#endif // ISHAVE_ASTRACAMERA

    /*************************************************************************************************************************/
    //��ʾPCD��������
    //auto cloudPtr = open3d::io::CreatePointCloudFromFile("D:\\LaserThermalReconstruction\\PCD\\R_CFRP_gray(20211028).pcd");
    ////auto voxelMesh = open3d::geometry::VoxelGrid::CreateFromPointCloud(*cloudPtr, 0.001);
    //
    ////auto mesh = open3d::geometry::TriangleMesh::CreateFromPointCloudBallPivoting(*cloudPtr, { 0.005, 0.01, 0.02, 0.04 });
    ////mesh->SamplePointsPoissonDisk(3000);
    //open3d::visualization::DrawGeometries({ cloudPtr }, "Open3D", 640, 480, 50, 50, false, false, true);
    //return 0;
    /*************************************************************************************************************************/

    /*************************************************************************************************************************/
    //�����Ǳ궨���ݾֲ���ֵ��
    //string inputFile  = "F:\\00reconstrcuction\\04data\\20220413calibration\\thermal_";
    //string outputFile = "F:\\00reconstrcuction\\04data\\20220413calibration\\thermal";
    //AdaptiveThreshold(inputFile, outputFile);
    //return 0;
    /*************************************************************************************************************************/


    /*************************************************************************************************************************/
    //�궨�����Ǻ����������������
    //CalibrateEyeInHand("F:\\00reconstrcuction\\04data\\20220413calibration");
    //return 0;
    /*************************************************************************************************************************/ 


    /*************************************************************************************************************************/
    H_Gripper2Thermal = H_Thermal2Gripper.inv();
    str2Mat(robotPositionCamera, ToolPoseCamera);
    str2Mat(robotPositionThermal, ToolPoseThermal);

    //��е��λ��Ϊŷ����-��ת����(camera)
    for (int i = 0; i < robotPositionCamera.size(); i++) {
        string depthImagePath = dataPath + "\\depth\\000" + to_string(i) + ".png";
        depthImageVec.push_back(imread(depthImagePath, IMREAD_ANYDEPTH));

        string colorImagePath = dataPath + "\\color\\000" + to_string(i) + ".jpg";
        colorImageVec.push_back(imread(colorImagePath));

        Mat H_Gripper2Base_camera = attitudeVectorToMatrix(ToolPoseCamera.row(i), false, "xyz");
        Mat cameraPosition = H_Gripper2Base_camera * H_Camera2Gripper * (Mat_<double>(4, 1) << 0, 0, 0, 1);

        cameraPositionVec.push_back(Eigen::Vector3d(cameraPosition.at<double>(0, 0), cameraPosition.at<double>(1, 0), cameraPosition.at<double>(2, 0)));
        H_Gripper2Base_cameraVec.push_back(H_Gripper2Base_camera);
        H_Base2Gripper_cameraVec.push_back(H_Gripper2Base_camera.inv());
    }

    //��е��λ��Ϊŷ����-��ת����(thermal)
    for (int i = 0; i < robotPositionThermal.size(); i++) {
        Mat H_Gripper2Base_thermal = attitudeVectorToMatrix(ToolPoseThermal.row(i), false, "xyz");
        Mat thermalPosition = H_Gripper2Base_thermal * H_Thermal2Gripper * (Mat_<double>(4, 1) << 0, 0, 0, 1);
        Mat H_Base2Gripper_thermal = H_Gripper2Base_thermal.inv();

        thermalPositionVec.push_back(Eigen::Vector3d(thermalPosition.at<double>(0, 0), thermalPosition.at<double>(1, 0), thermalPosition.at<double>(2, 0)));
        H_Base2Gripper_thermalVec.push_back(H_Base2Gripper_thermal);

        string thermalPath = dataPath + "\\00" + to_string(i + 4) + ".jpg";
        Mat thermalImage = imread(thermalPath);
        cv::flip(thermalImage, thermalImage, 0);
        thermalImageVec.push_back(thermalImage);
    } 

    for (int i = 0; i < robotPositionCamera.size(); i++) {
        //��ȡ��ǰ�����ͼ��
        Mat depthImage = depthImageVec[i];
        //ƴ�����ͼΪ��ά�������ݣ�ƥ���������ͼ����ά����
        for (int row = 0; row < depthImage.rows; row++) {
            for (int col = 0; col < depthImage.cols; col++) {
                Eigen::Vector3d pointTemp;
                Eigen::Vector3d colorTemp = {0, 0, 0};
                ushort depthNum = depthImage.at<ushort>(row, col);
                if (row > 0 && depthNum == 0) {
                    if (depthImage.at<ushort>(row - 1, col) > 0) {
                        int row_ = row;
                        while (row_ < depthImage.rows && depthImage.at<ushort>(row_, col) == 0) {
                            row_++;
                        }
                        if (row_ < depthImage.rows) {
                            depthImage.at<ushort>(row, col) = (depthImage.at<ushort>(row_, col) - depthImage.at<ushort>(row - 1, col)) / (double)(row_ - row + 1) + depthImage.at<ushort>(row - 1, col);
                            depthNum = depthImage.at<ushort>(row, col);
                        }
                    }
                }
                if (depthNum > MIN_DISTANCE && depthNum < MAX_DISTANCE) {
                    //�����������µ�����
                    double x_camera, y_camera, z_camera;
                    astraCameraD2C->convertDepthToCamrera(col, row, depthNum, x_camera, y_camera, z_camera);
                    cv::Mat p_camera{ x_camera, y_camera, z_camera, 1.0 };

                    //�����е���µ�������Ϣ
                    cv::Mat p_gripper = H_Camera2Gripper * p_camera;
                    //��������µ�������Ϣ
                    cv::Mat p_base = H_Gripper2Base_cameraVec[i] * p_gripper;
                    if (p_base.at<double>(2) < -300)
                        continue;
                    if (p_base.at<double>(1) < -950 || p_base.at<double>(1) > -400)
                        continue;
                    if (p_base.at<double>(0) < -500 || p_base.at<double>(0) > 500)
                        continue;
                    //�����������㵽��������ļ��γ���
                    pointTemp << p_base.at<double>(0), p_base.at<double>(1), p_base.at<double>(2);
                   
                    Eigen::Vector3d colorThermal(0.0, 0.0, 0.0);
                    Eigen::Vector3d colorCamera(0.0, 0.0, 0.0);

                    GetColorFromImageVec(colorThermal, p_base, "thermal");
                    //GetColorFromImageVec(colorCamera, p_base, "camera");
                    
                    double weightCamera = 0.0;
                    double weightThermal = 1.0;
                    colorTemp = weightCamera * colorCamera + weightThermal * colorThermal;
                    pcl_ptr->points_.push_back(pointTemp);
                    pcl_ptr->colors_.push_back(colorTemp);
                }
            }
        }
    }

    open3d::io::WritePointCloudToPLY("ship.ply", *pcl_ptr);
    open3d::io::WritePointCloudToPCD("ship.pcd", *pcl_ptr);

    Visualizer vis0(pcl_ptr);
    vis0.Run();

    return 0;
}

void GetColorFromImageVec(Eigen::Vector3d& colorTmp, const cv::Mat& p_base, string thermalOrCamera = "thermal") {
    colorTmp << 0.0, 0.0, 0.0;
    Eigen::Vector3d pointTemp(p_base.at<double>(0), p_base.at<double>(1), p_base.at<double>(2));
    vector<Image> imageVec;
    if (thermalOrCamera == "thermal") {
        for (int j = 0; j < thermalPositionVec.size(); j++) {
            int distanceTmp = (thermalPositionVec[j] - pointTemp).norm();
            //�����������µ�������Ϣ
            cv::Mat p_thermal = H_Thermal2Gripper.inv() * H_Base2Gripper_thermalVec[j] * p_base;
            //���������Ƕ�Ӧ����������
            p_thermal = ThermalMatrix * p_thermal;

            int u = p_thermal.at<double>(0, 0) / p_thermal.at<double>(2, 0);
            int v = p_thermal.at<double>(1, 0) / p_thermal.at<double>(2, 0);
            if (u > 0 && u < 640 && v > 0 && v < 480) {
                imageVec.push_back(Image(j, u, v, distanceTmp));
            }
        }
    }
    else if (thermalOrCamera == "camera") {
        for (int j = 0; j < cameraPositionVec.size(); j++) {
            int distanceTmp = (cameraPositionVec[j] - pointTemp).norm();
            //�����������µ�������Ϣ
            cv::Mat p_camera = H_Camera2Gripper.inv() * H_Base2Gripper_cameraVec[j] * p_base;
            //���������Ƕ�Ӧ����������
            p_camera = CameraMatrix * p_camera;

            int u = p_camera.at<double>(0, 0) / p_camera.at<double>(2, 0);
            int v = p_camera.at<double>(1, 0) / p_camera.at<double>(2, 0);
            if (u > 0 && u < 640 && v > 0 && v < 480) {
                imageVec.push_back(Image(j, u, v, distanceTmp));
            }
        }
    }

    //�鿴�õ����Ƿ���ƥ�������������ͼ
    if (imageVec.empty()) {
        //���û��ƥ�������ͼ��������Ϊ��ɫ
        colorTmp << 1, 1, 1;
    }
    else {
        //�����ƥ�������ͼ��������ͼ��Ȩ�ں�
        sort(imageVec.begin(), imageVec.end(), [=](Image img1, Image img2) {
            return img1.distance < img2.distance; });

        int differencValue = 20;
        while (imageVec.back().distance - imageVec.front().distance > differencValue) {
            imageVec.pop_back();
        }
        int distanceSum = differencValue;
        for (auto image_ : imageVec) {
            distanceSum += image_.distance;
        }
        int meanDistance = distanceSum / imageVec.size();

        for (auto image_ : imageVec) {
            double weight = (meanDistance - image_.distance) / (double)differencValue;
            Mat img;
            if (thermalOrCamera == "thermal") {
                img = thermalImageVec[image_.imageIndex];
            }
            else if (thermalOrCamera == "camera") {
                img = colorImageVec[image_.imageIndex];
            }
            int u = image_.u;
            int v = image_.v;
            Eigen::Vector3d colorTemp_(img.at<Vec3b>(v, u)[2] / 255.0, img.at<Vec3b>(v, u)[1] / 255.0, img.at<Vec3b>(v, u)[0] / 255.0);

            colorTmp += colorTemp_ * weight;
        }
    }
}