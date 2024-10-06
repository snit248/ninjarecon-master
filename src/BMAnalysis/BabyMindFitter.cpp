#include "BabyMindFitter.hpp"

BabyMindFitter::BabyMindFitter() {
    for (int iview = 0; iview < 2; iview++) {
        hit_graph[iview] = new TGraphAsymmErrors();
        linear[iview] = new TF1(Form("linear_%d", iview), "[0] * x + [1]", -2000., 2000.);
    }
}

BabyMindFitter::~BabyMindFitter() {
    for (int iview = 0; iview < 2; iview++) {
        delete linear[iview];
        delete hit_graph[iview];
    }
}

std::vector<double> BabyMindFitter::Fit(const B2TrackSummary* track) {
    std::vector<double> param(4, -9999);
    CollectHits(track);

    if (hits.empty()) return param;

    FillGraph();

    for (int iview = 0; iview < 2; iview++) {
        hit_graph[iview]->Fit(linear[iview], "Q");
        param[iview] = linear[iview]->GetParameter(0);
        param[iview + 2] = linear[iview]->GetParameter(1);
    }

    return param;
}

void BabyMindFitter::CollectHits(const B2TrackSummary* track) {
    hits.clear();
    auto it_cluster = track->BeginCluster();
    while (const auto* cluster = it_cluster.Next()) {
        auto it_hit = cluster->BeginHit();
        while (const auto* hit = it_hit.Next()) {
            if (hit->GetDetectorId() == B2Detector::kBabyMind) {
                hits.push_back(hit);
            }
        }
    }
}

void BabyMindFitter::FillGraph() {
    for (int iview = 0; iview < 2; iview++) {
        hit_graph[iview]->Set(0); 
    }

    for (const auto* hit : hits) {
        const TVector3& pos = hit->GetScintillatorPosition().GetValue();
        if (hit->GetView() == B2View::kSideView) {
            hit_graph[B2View::kSideView]->SetPoint(hit_graph[B2View::kSideView]->GetN(), pos.Z(), pos.Y());
        } else if (hit->GetView() == B2View::kTopView) {
            hit_graph[B2View::kTopView]->SetPoint(hit_graph[B2View::kTopView]->GetN(), pos.Z(), pos.X());
        }
    }
}