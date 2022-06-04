#include "d2common/d2vinsframe.h"
#include "../d2vins_params.hpp"
#include "landmark_manager.hpp"
#include "d2vinsstate.hpp"
#include <swarm_msgs/Odometry.h>
#include <ceres/ceres.h>
#include "../visualization/visualization.hpp"
#include "solver/SolverWrapper.hpp"

using namespace Eigen;
using D2Common::VisualImageDescArray;

namespace D2VINS {
class Marginalizer;
class D2Estimator {
protected:
    //Internal states
    bool initFirstPoseFlag = false;   
    D2EstimatorState state;
    IMUBuffer imubuf;
    std::map<int, IMUBuffer> remote_imu_bufs;
    std::map<int, Swarm::Odometry> last_prop_odom; //last imu propagation odometry
    Marginalizer * marginalizer = nullptr;
    SolverWrapper * solver = nullptr;
    int solve_count = 0;
    int current_landmark_num = 0;
    std::set<int> used_camera_sets;
    std::vector<LandmarkPerId> margined_landmarks;
    std::map<int, bool> relative_frame_is_used;
    int self_id;
    int frame_count = 0;
    D2Visualization visual;
    
    //Internal functions
    bool tryinitFirstPose(VisualImageDescArray & frame);
    void addFrame(VisualImageDescArray & _frame);
    void addFrameRemote(const VisualImageDescArray & _frame);
    void solve();
    void setupImuFactors();
    void setupLandmarkFactors();
    void addIMUFactor(FrameIdType frame_ida, FrameIdType frame_idb, IntegrationBase* _pre_integration);
    void setStateProperties();
    void setupPriorFactor();
    std::pair<bool, Swarm::Pose> initialFramePnP(const VisualImageDescArray & frame, 
        const Swarm::Pose & initial_pose);
    void addSldWinToFrame(VisualImageDescArray & frame);
    void addRemoteImuBuf(int drone_id, const IMUBuffer & imu_buf);
    bool isLocalFrame(FrameIdType frame_id) const;
    bool isMain() const;
public:
    D2Estimator(int drone_id);
    void inputImu(IMUData data);
    bool inputImage(VisualImageDescArray & frame);
    void inputRemoteImage(VisualImageDescArray & frame);
    void solveinDistributedMode();
    Swarm::Odometry getImuPropagation() const;
    Swarm::Odometry getOdometry() const;
    Swarm::Odometry getOdometry(int drone_id) const;
    void init(ros::NodeHandle & nh);
    D2EstimatorState & getState();
    std::vector<LandmarkPerId> getMarginedLandmarks() const;
};
}