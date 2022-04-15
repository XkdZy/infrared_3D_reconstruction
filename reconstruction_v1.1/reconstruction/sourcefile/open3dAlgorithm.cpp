#include "open3dAlgorithm.h"

//��̬��ŷ����ת��Ϊ��ת�������ڸ���任
Eigen::Matrix3d eulerAnglesToRotationMatrix(Eigen::Vector3d& theta) {
	// Calculate rotation about x axis
	for (int i = 0; i < 3; i++)
		theta[i] = theta[i] / 57.296;

	Eigen::Matrix3d R_x;
	R_x <<	1,	0,				0,
			0,	cos(theta[0]),	-sin(theta[0]),
			0,	sin(theta[0]),	cos(theta[0]);

	// Calculate rotation about y axis
	Eigen::Matrix3d R_y;
	R_y <<	cos(theta[1]),	0,	sin(theta[1]),
			0,				1,	0,
			-sin(theta[1]), 0,	cos(theta[1]);

	// Calculate rotation about z axis
	Eigen::Matrix3d R_z;
	R_z <<	cos(theta[2]),	-sin(theta[2]), 0,
			sin(theta[2]),	cos(theta[2]),	0,
			0,				0,				1;   

	// Combined rotation matrix
	return R_z * R_y * R_x;
}

//PCA  ���������Ƶ���������
void PCA(Eigen::MatrixXd& X, Eigen::MatrixXd& vec, Eigen::MatrixXd& val) {
	//����ÿһά�Ⱦ�ֵ
	Eigen::MatrixXd meanval = X.colwise().mean();
	Eigen::RowVectorXd meanvecRow = meanval;
	//������ֵ��Ϊ0
	X.rowwise() -= meanvecRow;

	//����Э�������C = XTX / n-1;
	Eigen::MatrixXd Cov;
	//C = X.adjoint() * X;
	//C = C.array() / X.rows() - 1;
	Cov = X.transpose() * X;

	//��������ֵ������������ʹ��selfadjont���ն��������㷨ȥ���㣬�����ò�����vec��val������������
	Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eig(Cov);

	//��ȡ����ֵ����������
	val = eig.eigenvalues();
	vec = eig.eigenvectors();

	if (vec(2, 0) > 0)
		vec = -vec;
}

void DownSampleAndEstimateNormals(std::shared_ptr<geometry::PointCloud> pointcloud, double voxelsize, int kNum = 300) {
	//���������²���
	auto downsamplePoint = pointcloud->VoxelDownSample(voxelsize);
	
	//����KDTree ����K��������           
	geometry::KDTreeFlann pointTree(*pointcloud);

	//����������������  300���ڵ����������
	for (int index = 0; index < downsamplePoint->points_.size(); index++) {
		//��������ʹ��KNN��������  Ѱ��ԭ�����е������
		std::vector<int> indices;
		std::vector<double> distance;
		pointTree.SearchKNN(downsamplePoint->points_[index], 1, indices, distance);
		indices.clear();
		distance.clear();

		//��������ʹ��KNN��������  Ѱ�Ҹ������kNum����������
		pointTree.SearchKNN(pointcloud->points_[indices[0]], kNum, indices, distance);

		//���������е�ֵ
		downsamplePoint->points_[index] = pointcloud->points_[indices[0]];

		//���K���ڵ㵽������
		Eigen::MatrixXd knearestPointData(kNum, 3);
		for (int i = 0; i < kNum; i++)
			knearestPointData.row(i) = pointcloud->points_[indices[i]];

		//PCA  �����������
		Eigen::MatrixXd vec, val;
		PCA(knearestPointData, vec, val);

		//��0��Ϊ����ֵ��С���У�Ҳ��������������Ӧ�ķ���
		downsamplePoint->normals_.push_back(vec.col(0));
	}
}