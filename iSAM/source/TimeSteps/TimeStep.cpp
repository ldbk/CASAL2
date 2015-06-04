/**
 * @file TimeStep.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 20/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science �2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

#include "TimeStep.h"

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>

#include "InitialisationPhases/Manager.h"
#include "Model/Model.h"
#include "Observations/Manager.h"
#include "Processes/Manager.h"

namespace niwa {

/**
 * Default Constructor
 */
TimeStep::TimeStep() {
  LOG_TRACE();

  parameters_.Bind<string>(PARAM_LABEL, &label_, "Label", "");
  parameters_.Bind<string>(PARAM_PROCESSES, &process_names_, "Processes", "");
}

/**
 * Validate our time step
 */
void TimeStep::Validate() {
  parameters_.Populate();
}

/**
 * Build our time step
 */
void TimeStep::Build() {

  // Get the pointers to our processes
  processes::Manager& process_manager = processes::Manager::Instance();
  for (string process_name : process_names_) {
    ProcessPtr process = process_manager.GetProcess(process_name);
    if (!process)
      LOG_ERROR_P(PARAM_PROCESSES) << ": process " << process_name << " does not exist. Have you defined it?";

    processes_.push_back(process);
  }
  LOG_FINE() << "Time step " << label_ << " has " << processes_.size() << " processes";

  /**
   * If we have a block build it
   */
  block_start_process_index_ = processes_.size();
  block_end_process_Index_ = processes_.size() - 1;
  for (unsigned i = 0; i < processes_.size(); ++i) {
    if (processes_[i]->process_type() == ProcessType::kMortality) {
      block_start_process_index_ = block_start_process_index_ == processes_.size() ? i : block_start_process_index_;
      block_end_process_Index_ = i;
    }
  }

  block_start_process_index_ = block_start_process_index_ == processes_.size() ? 0 : block_start_process_index_;
}

/**
 * Execute the time step during the initialisation phases
 */
void TimeStep::ExecuteForInitialisation(const string& phase_label) {
  LOG_FINEST() << "Executing for initialisation phase: " << phase_label << " with " << initialisation_block_executors_.size() << " executors";
  LOG_FINEST() << "initialisation_block_end_process_index_: " << initialisation_block_end_process_index_[phase_label];

  for (unsigned index = 0; index < initialisation_processes_[phase_label].size(); ++index) {
    initialisation_processes_[phase_label][index]->Execute(0u, "");
    if (initialisation_block_end_process_index_[phase_label] == index) {
      for (ExecutorPtr executor : initialisation_block_executors_) {
        executor->Execute();
      }
    }
  }
}

/**
 * Execute the time step
 */
void TimeStep::Execute(unsigned year) {
  LOG_TRACE();

  for (ExecutorPtr executor : executors_[year])
      executor->PreExecute();

  for (unsigned index = 0; index < processes_.size(); ++index) {
    if (index == block_start_process_index_) {
      for (ExecutorPtr executor : block_executors_[year])
        executor->PreExecute();
    }

    for(ExecutorPtr executor : process_executors_[year][index])
      executor->PreExecute();

    processes_[index]->Execute(year, label_);

    for(ExecutorPtr executor : process_executors_[year][index])
      executor->Execute();

    if (index == block_end_process_Index_) {
      for (ExecutorPtr executor : block_executors_[year])
        executor->Execute();
    }
  }

  for (ExecutorPtr executor : executors_[year])
    executor->Execute();
}

/**
 *
 */
void TimeStep::SubscribeToBlock(ExecutorPtr executor) {
  vector<unsigned> years = Model::Instance()->years();
  for (unsigned year : years)
    block_executors_[year].push_back(executor);
}

/**
 *
 */
void TimeStep::SubscribeToProcess(ExecutorPtr executor, unsigned year, string process_label) {
  LOG_TRACE();

  unsigned index = 1;
  bool index_defined = false;
  if (process_label.find("(") != string::npos) {
    vector<string> temp;
    boost::split(temp, process_label, boost::is_any_of("()"));
    if (temp.size() <= 1 )
      LOG_CODE_ERROR() << "temp.size() == 1: " << process_label;

    if (!utilities::To<unsigned>(temp[1], index))
      LOG_FATAL() << executor->location() << "the process index " << temp[1] << " could not be converted to an unsigned integer";
    process_label = temp[0];
    index_defined = true;
    LOG_FINEST() << "process_label: " << process_label << " | " << temp[0];
  }

  vector<ProcessPtr> matching;
  std::for_each(processes_.begin(), processes_.end(), [&matching, process_label](ProcessPtr p){ if (p->label() == process_label) matching.push_back(p); });
  if (matching.size() > 1 && !index_defined)
    LOG_FATAL() << executor->location() << "the process " << process_label << " was defined multiple times in the time step " << label_
        << " but the observation on it did not have an index (e.g" << process_label << "(1)";

  if (index == 0)
    LOG_FATAL() << executor->location() << "the process " << process_label << " index was 0. It should be a 1 based array, not 0 based";
  --index;

  if (matching.size() == 0)
    LOG_FATAL() << executor->location() << "the process " << process_label << " was not defined in the time_step " << label_;
  if (index >= matching.size())
    LOG_FATAL() << executor->location() << "the process index of " << index << " is too high. Index range is 1-" << matching.size();

  process_executors_[year][index].push_back(executor);
}

/**
 *
 */
void TimeStep::SetInitialisationProcessLabels(const string& initialisation_phase_label, vector<string> process_labels_) {
  initialisation_process_labels_[initialisation_phase_label] = process_labels_;
}

/**
 *
 */
void TimeStep::BuildInitialisationProcesses() {
  for (auto iter : initialisation_process_labels_) {
    for (string process_label : iter.second) {
      initialisation_processes_[iter.first].push_back(processes::Manager::Instance().GetProcess(process_label));
    }

    initialisation_block_end_process_index_[iter.first]   = iter.second.size() - 1;
    for (unsigned i = 0; i < initialisation_processes_[iter.first].size(); ++i) {
      if (initialisation_processes_[iter.first][i]->process_type() == ProcessType::kMortality) {
        initialisation_block_end_process_index_[iter.first] = i;
      }
    }
  }
}
} /* namespace niwa */
