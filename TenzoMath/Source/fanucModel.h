#ifndef FANUC_MODEL
#define FANUC_MODEL

#include "RoboModel.h"


/**
* \brief class for matematics of robot manipulator Fanuc M20ia based on Denavit-Hartenberg parameters
*/
class FanucModel : RoboModel
{
public:
    static constexpr double PI = 3.14159265;


    /**
    * \brief default constructor with Fanuc M20ia parameters
    */
    FanucModel();

    /**
    * \brief function for solving forward kinematic task for Fanuc M20ia
    * \param[in] inputjoints joints angles
    * \return coordinates of end-effector in world frame: x, y, z in mm and w, p, r in radians
    */
    cv::Mat fanucForwardTask(const std::array<double, 6>& inputjoints);

    /**
     * \brief returns _ToCamera transformation matrix
     * \return 4x4 transformation matrix
     */
    cv::Mat getToCamera() const;

    /**
    * \brief returns _ToSixth transformation matrix
    * \return 4x4 transformation matrix
    */
    cv::Mat getToSixth() const;

    /**
     * \brief returns Denavit-Hartenberg parameters of Fanuc M20ia
     * \return D-H parameters
     */
    std::vector<DhParameters> getDhParameters() const;

    /**
     * \brief calculates 6 Cartesian coordinates from transformation matrix
     * \param[in] transformMatrix 4x4 matrix
     * \return 3 coordinates x, y, z and 3 angles w, p, r
     */
    static std::array<double, 6> getCoordsFromMat(cv::Mat transformMatrix);

    /**
     * \brief solves inverse kinematic task 
     * \param[in] coord 6 Cartesian coordinates x, y, z, w, p, r
     * \return matrix of solutions
     */
    cv::Mat fanucInverseTask(const std::array<double, 6>& coord) const;

    /**
    * \brief solves inverse kinematic task
    * \param[in] coord 6 Cartesian coordinates x, y, z, w, p, r
    * \return matrix of solutions
    */
    cv::Mat fanucInverseTaskNew(const std::array<double, 6>& coord) const;

    /**
    * \brief calculates rotation matrix of end-effector. input angles given in radians
    * \param[in] w angle of rotation around x axis
    * \param[in] p angle of rotation around y axis
    * \param[in] r angle of rotation around z axis
    * \return rotation matrix 3*3
    */
    static cv::Mat rotMatrix(const double& w, const double& p, const double& r);

    /**
    * \brief function to calculate three rotation angles from transformation matrix
    * \param[in] p6 transofrmation matrix(4x4) or rotation matrix(3x3)
    * \return three Tait-Bryan angles
    */
    static std::array<double, 3> anglesFromMat(const cv::Mat p6);


private:
    /**
    * \brief function for conversion joints angles to Denavit-Hartenberg generalized angles
    * \param[in] j joints angles
    * \return Denavit-Hartenberg generalized angles
    */
    static std::vector<double> jointsToQ(const std::array<double, 6>& j);

    /**
     * \brief rotation matrix from i-th frame to (i + 1)-th frame
     * \param[in] alpha one of D-H parameters 
     * \param[in] q generalized D-H angle 
     * \return rotation matrix 3x3
     */
    cv::Mat qi(const double& alpha, const double& q) const;

    /**
     * transformation matrix
     */
    const cv::Mat _toCamera, _toSixth, _forMovingToCamera;
};

#endif // FANUC_MODEL
