#ifndef BABYMIND_FITTER_HPP
#define BABYMIND_FITTER_HPP

#include <vector>
#include <TGraphAsymmErrors.h>
#include <TF1.h>
#include <B2TrackSummary.hh>
#include <B2ClusterSummary.hh>
#include <B2HitSummary.hh>

class BabyMindFitter {
public:
    BabyMindFitter();
    ~BabyMindFitter();
    std::vector<double> Fit(const B2TrackSummary* track);

private:
    void CollectHits(const B2TrackSummary* track);
    void FillGraph();

    std::vector<const B2HitSummary*> hits;
    TGraphAsymmErrors* hit_graph[2];
    TF1* linear[2];
};

#endif