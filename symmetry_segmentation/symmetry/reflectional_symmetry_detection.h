// Copyright 2017 Aleksandrs Ecins
// Licensed under GPLv2+
// Refer to the LICENSE.txt file included.

#ifndef REFLECTIONAL_SYMMETRY_DETECTION_H
#define REFLECTIONAL_SYMMETRY_DETECTION_H

#include <symmetry/reflectional_symmetry.hpp>
#include <occupancy_map.hpp>

namespace sym
{
  //----------------------------------------------------------------------------
  // Symmetry detection parameters
  //----------------------------------------------------------------------------
  
  struct ReflSymDetectParams
  {
    // Downsample parameters
    float voxel_size = 0.0f;
    
    // Initialization parameters
    int num_angle_divisions = 5;
    float flatness_threshold = 0.005f;
    
    // Refinement parameters
    int refine_iterations = 20;
    
    // Symmetry scoring parameters
    float max_correspondence_reflected_distance = 0.01f;
    float min_occlusion_distance = 0.01f;
    float max_occlusion_distance = 0.2f;
    float min_inlier_normal_angle = pcl::deg2rad(10.0f);
    float max_inlier_normal_angle = pcl::deg2rad(15.0f);

    // Symmetry filtering parameters
    float max_occlusion_score = 0.01f;
    float min_cloud_inlier_score = 0.2f;
    float min_corresp_inlier_score = 4.0f;
    
    // Similarity parameters for merging
    float symmetry_min_angle_diff       = pcl::deg2rad(7.0);  // Two symmetries are considered similar if the angle between their normals is less than this threshold
    float symmetry_min_distance_diff    = 0.02f;              // Two symmetries are considered similar if the difference between their distances to origin is less than this threshold
    float max_reference_point_distance  = 0.3f;               // Maximum distance between the reference points of two symmetries that can be merged
  };
  
  //----------------------------------------------------------------------------
  // Symmetry detection merging
  //----------------------------------------------------------------------------

  /** \brief Merge symemtry hypotheses that are similar enough.
   *  \param[in]  symmetry_reference_points   reference points for input symmetries
   *  \param[in]  symmetries                  input symmetries
   *  \param[in]  indices                     indices of the symmetries used
   *  \param[in]  symmetry_scores             symmetry scores
   *  \param[in]  occlusion_scores            occlusion scores
   *  \param[out] merged_sym_ids              ids of symmetries after merging
   *  \param[in]  max_normal_angle_diff       maximum angle between the normals of two distinct symmetries that will be merged
   *  \param[in]  max_distance_diff           minimum distance between two symmetries that will be merged
   *  \param[in]  max_reference_point_distance  maximum distance between the reference points of two symmetries that can be merged
   */
  inline
  void mergeDuplicateReflSymmetries ( const std::vector<sym::ReflectionalSymmetry> &symmetries,
                                      const std::vector<Eigen::Vector3f> &symmetry_reference_points,
                                      const std::vector<int> &indices,
                                      const std::vector<float> &occlusion_scores,
                                      std::vector<int> &merged_sym_ids,
                                      const float max_normal_angle_diff = pcl::deg2rad(10.0f),
                                      const float max_distance_diff = 0.01f,
                                      const float max_reference_point_distance = -1.0f
                                    );
  
  /** \brief Merge symemtry hypotheses that are similar enough.
   *  \param[in]  symmetry_reference_points   reference points for input symmetries
   *  \param[in]  symmetries                  input symmetries
   *  \param[in]  symmetry_scores             symmetry scores
   *  \param[in]  occlusion_scores            occlusion scores
   *  \param[out] merged_sym_ids              ids of symmetries after merging
   *  \param[in]  max_normal_angle_diff       maximum angle between the normals of two distinct symmetries that will be merged
   *  \param[in]  max_distance_diff           minimum distance between two symmetries that will be merged
   *  \param[in]  max_reference_point_distance  maximum distance between the reference points of two symmetries that can be merged
   */
  inline
  void mergeDuplicateReflSymmetries ( const std::vector<sym::ReflectionalSymmetry> &symmetries,
                                      const std::vector<Eigen::Vector3f> &symmetry_reference_points,
                                      const std::vector<float> &occlusion_scores,
                                      std::vector<int> &merged_sym_ids,
                                      const float max_normal_angle_diff = pcl::deg2rad(10.0f),
                                      const float max_distance_diff = 0.01f,
                                      const float max_reference_point_distance = -1.0f
                                    );
  
  //----------------------------------------------------------------------------
  // Symmetry detection class
  //----------------------------------------------------------------------------
  
  /** \brief Reflectional symmmetry detection. */
  template <typename PointT>
  class ReflectionalSymmetryDetection
  {
  public:
    
    /** \brief Empty constructor. */
    ReflectionalSymmetryDetection ();
    
    /** \brief Constructor with custom parameters. */
    ReflectionalSymmetryDetection (const ReflSymDetectParams &params);
    
    /** \brief Destructor. */
    ~ReflectionalSymmetryDetection ();
    
    /** \brief Provide a pointer to the input pointcloud.
     *  \param cloud the const boost shared pointer to a pointcloud
     */
    inline
    void setInputCloud (const typename pcl::PointCloud<PointT>::ConstPtr &cloud);
    
    /** \brief Provide a pointer to the input scene occupancy map.
     *  \param cloud the const boost shared pointer to an occupancy map
     */
    inline
    void setInputOcuppancyMap  (const OccupancyMapConstPtr &occupancy_map);

    /** \brief Set initial symmetries.
     *  \param initial_symmetries   vector of initial symmetries
     */
    inline
    void setInputSymmetries  (const std::vector<sym::ReflectionalSymmetry> &symmetries_initial);

    /** \brief Set detection parameters.
     *  \param params detection parameters
     */
    inline
    void setParameters (const ReflSymDetectParams &params);
    
    /** \brief Detect reflectional symmetries in the input pointcloud. */
    inline bool detect ();

    /** \brief Filter detected symmetries. */
    inline void filter ();
    
    /** \brief Filter detected symmetries. */
    inline void merge ();
    
    /** \brief Get all of the refined symmetries as well as indices of filtered
     * and merged symmetries.
     *  \param[out] symmetries              vector of all refined symmetries
     *  \param[out] symmetry_filtered_ids   indices of filtered symmetries
     *  \param[out] symmetry_merged_ids     indices of merged symmetries
     */
    inline
    void getSymmetries  ( std::vector<sym::ReflectionalSymmetry> &symmetries,
                          std::vector<int> &symmetry_filtered_ids,
                          std::vector<int> &symmetry_merged_ids );
 
    /** \brief Get scores for the refined symmetries.
     *  \param[out] symmetry_scores           symmetry scores
     *  \param[out] occlusion_scores_         symmetry occlusion scores
     *  \param[out] symmetry_inlier_scores_   symmetry inlier scores
     */
    inline
    void getScores  ( std::vector<float> &occlusion_scores,
                      std::vector<float> &cloud_inlier_scores,
                      std::vector<float> &corresp_inlier_scores  );
    
    /** \brief Get point symmetry and occlusion scores for the refined symmetries.
     *  \param[out] cloud_ds                downsampled input cloud
     *  \param[out] correspondences         symmetric correspondences
     *  \param[out] point_symmetry_scores   symmetry scores
     *  \param[out] point_occlusion_scores  occlusion scores
     */
    inline
    void getPointScores ( typename pcl::PointCloud<PointT>::Ptr &cloud_ds,
                          std::vector<pcl::Correspondences> &correspondences,
                          std::vector<std::vector<float> > &point_symmetry_scores,
                          std::vector<std::vector<float> > &point_occlusion_scores  );
    
  private:
        
    /** \brief Detection parameters. */
    ReflSymDetectParams params_;
    
    /** \brief Input cloud. */
    typename pcl::PointCloud<PointT>::ConstPtr cloud_;

    /** \brief Input cloud. */
    Eigen::Vector3f cloud_mean_;
    
    /** \brief Scene occupancy map. */
    OccupancyMapConstPtr occupancy_map_;
    
    /** \brief Downsampled input cloud. */
    typename pcl::PointCloud<PointT>::Ptr cloud_ds_;
        
    /** \brief Symmetric correspondences. */
    std::vector<pcl::Correspondences> correspondences_;
    
    /** \brief Refined symmetries. */
    std::vector<sym::ReflectionalSymmetry> symmetries_initial_;

    /** \brief Refined symmetries. */
    std::vector<sym::ReflectionalSymmetry> symmetries_refined_;    
        
    /** \brief Symmetry scores. */
    std::vector<float> occlusion_scores_;

    /** \brief Symmetry inlier scores. */
    std::vector<float> cloud_inlier_scores_;
    
    /** \brief Symmetry scores. */
    std::vector<float> corresp_inlier_scores_;

    /** \brief Pointwise symmetry scores */
    std::vector<std::vector<float> > point_symmetry_scores_;
    
    /** \brief Pointwise occlusion scores */
    std::vector<std::vector<float> > point_occlusion_scores_;
    
    /** \brief Indices of the filtered symmetries. */
    std::vector<int> symmetry_filtered_ids_;
    
    /** \brief Indices of the merged symmetries. */
    std::vector<int> symmetry_merged_ids_;
  };
}

#endif  // REFLECTIONAL_SYMMETRY_DETECTION_H