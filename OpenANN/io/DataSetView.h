#pragma once

#include <Eigen/Dense>
#include <vector>
#include <OpenANN/io/DataSet.h>

namespace OpenANN {

class Learner;


/**
 * @class DataSetView
 *
 * An index-based dataset wrapper for representing efficient dataset views on 
 * any OpenANN::DataSet instance. Its only operating on an index container 
 * that points to the current accessable instances from the reference 
 * OpenANN::DataSet.
 *
 * Can be used for splitting and merging any DataSet& partitions.
 *
 * No manually memory de- and allocation is needed to handle internal pointers.
 */

class DataSetView : public DataSet 
{
public:
  /**
   * Create an new instance on a given DataSetView (copy constructor)
   *
   * @param dataset source instance whose internal data is copied
   */
  DataSetView(const DataSetView& dataset);
  
  /**
   * Create an empty view on a given OpenANN::DataSet without samples
   *
   * @param dataset source instance that is referenced by this view
   */
  DataSetView(DataSet& dataset) : dataset(&dataset) 
  {}
 
  /**
   * Create a new dataset view on a given OpenANN::DataSet by using an 
   * STL-like interface on std::vector<int>::iterators for indices.
   *
   * @param dataset source instance that is referenced by this view
   * @param index_begin iterator to the first index of an int stl container
   * @param index_end iterator to the last index of an int stl container
   */
  template<typename InputIt>
  DataSetView(DataSet& dataset, InputIt index_begin, InputIt index_end) 
  :  indices(index_begin, index_end), dataset(&dataset)
  {}

  /**
   * Destructor for DataSetView (does nothing internally)
   */
  virtual ~DataSetView() {}

  /**
   * See OpenANN::DataSet::samples()
   */
  virtual int samples();

  /**
   * See OpenANN::DataSet::inputs()
   */
  virtual int inputs();

  /**
   * See OpenANN::DataSet::inputs()
   */
  virtual int outputs();

  /**
   * See OpenANN::DataSet::getInstance(int)
   */
  virtual Eigen::VectorXd& getInstance(int i);

  /**
   * See OpenANN::DataSet::getTarget(int)
   */
  virtual Eigen::VectorXd& getTarget(int i);

  /**
   * See OpenANN::DataSet::finishIteration(OpenANN::Learner&)
   */
  virtual void finishIteration(Learner& learner);

  /**
   * Shuffle the order of instances within this dataset view
   *
   * @return this instance shuffled.
   */
  virtual DataSetView& shuffle();


private:
  // indices from the original dataset that are related to this subview.
  std::vector<int> indices;

  // reference to the original dataset interface
  DataSet* dataset;

  // friend declaration for direct indices access
  friend void merge(DataSetView& merging, std::vector<DataSetView>& groups);
};

/**
 * Split the current DataSet into a specific number of DataSetView groups
 *
 * @param groups std::vector reference that will be filled with new dataset 
 *      views.
 * @param dataset reference for internal instances of each dataset view
 * @param number_of_groups the number of DataSetView generated by this call
 * @param shuffling if so, the split is called on a shuffled version of the 
 *      reference dataset
 */
void split(std::vector<DataSetView>& groups, DataSet& dataset, 
    int number_of_groups, bool shuffling = true);


/**
 * Split the current DataSet into two DataSetViews where the number of containing 
 * instances are controlled by a ratio flag.
 *
 * @param groups std::vector reference that will be filled with two dataset views.
 * @param dataset reference for internal instances of each dataset view
 * @param ratio sets the ratio of the sample count between the two sets
 *   - groups.front().size() == ratio * dataset.samples()
 *   - groups.back().size() == (1.0 - ratio) * dataset.samples()
 * @param shuffling if so, the split is called on a shuffled version of the 
 *     reference dataset
 */
void split(std::vector<DataSetView>& groups, DataSet& dataset, 
    double ratio = 0.5, bool shuffling = true);

/**
 * Merge all viewing instances from a DataSetView into another existing one.
 *
 * @param merging the destination DataSetView that will contain later all 
 * instances from the group
 * 
 * @param groups number of DataSetView that should be merged into the destination.
 */
void merge(DataSetView& merging, std::vector<DataSetView>& groups);




} // namespace OpenANN