#include "GShareBranchPredictor.h"

GShareBranchPredictor::GShareBranchPredictor(String name, core_id_t core_id, UInt32 entries, UInt32 b_history)
    : BranchPredictor(name, core_id)
    , m_predictors(entries, SaturatingPredictor<2>(0))
    , m_num_entries(entries)
    , g_history(0)
{
    h_mask = 1;

    LOG_PRINT_WARNING("entries: %d",  m_num_entries);

    for (unsigned int i = 0; i < b_history; i++) 
        h_mask = h_mask * 2;

    h_mask = h_mask - 1;

    LOG_PRINT_WARNING("Gshare: %d", h_mask);
}

GShareBranchPredictor::~GShareBranchPredictor() {
    m_predictors.clear();
}

bool GShareBranchPredictor::predict(IntPtr ip, IntPtr target) {

    return true;
}


BranchPredictorReturnValue GShareBranchPredictor::lookup(IntPtr ip, IntPtr target) {

    UInt32 index = (ip ^ g_history) & h_mask;
    BranchPredictorReturnValue ret = { 0, 0, 0, BranchPredictorReturnValue::InvalidBranch };

    ret.hit = 1;
    ret.prediction = m_predictors[index].predict();

    return ret;
}

void GShareBranchPredictor::update(bool predicted, bool actual, IntPtr ip, IntPtr target) {

    updateCounters(predicted, actual);

    UInt32 index = (ip ^ g_history) & h_mask;

    g_history = (g_history << 1) | actual;

    m_predictors[index].update(actual);
}
