// system includes
#include <vector>
#include <numeric>
#include <algorithm>
#include <iostream>
#include <iomanip>  // 追加

// boost includes
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

// root includes
#include <TGraphErrors.h>
#include <TGraphAsymmErrors.h>
#include <TF1.h>
#include <TString.h>
#include <TCanvas.h>

// B2 includes
#include <B2Reader.hh>
#include <B2Writer.hh>
#include <B2Enum.hh>
#include <B2Dimension.hh>
#include <B2SpillSummary.hh>
#include <B2BeamSummary.hh>
#include <B2HitSummary.hh>
#include <B2VertexSummary.hh>
#include <B2ClusterSummary.hh>
#include <B2TrackSummary.hh>
#include <B2EventSummary.hh>
#include <B2EmulsionSummary.hh>
#include <B2Pdg.hh>
#include "NTBMSummary.hh"

#include "TrackMatch.hpp"

namespace logging = boost::log;

// BabyMINDトラックの初期位置と角度を計算する関数
std::vector<double> FitBabyMind(const B2TrackSummary* track) {
    std::vector<double> param(4, -9999); // 初期化：不正な値を格納
    std::vector<const B2HitSummary*> hits;

    // BabyMINDのヒットを収集
    auto it_cluster = track->BeginCluster();
    while (const auto* cluster = it_cluster.Next()) {
        auto it_hit = cluster->BeginHit();
        while (const auto* hit = it_hit.Next()) {
            if (hit->GetDetectorId() == B2Detector::kBabyMind) {
                hits.push_back(hit);
            }
        }
    }

    if (hits.empty()) {
        return param; // ヒットがなければそのまま返す
    }

    // 角度をフィットするためのグラフを作成
    TGraphAsymmErrors* hit_graph[2];
    TF1* linear[2];

    for (int iview = 0; iview < 2; iview++) {
        hit_graph[iview] = new TGraphAsymmErrors();
        linear[iview] = new TF1(Form("linear_%d", iview), "[0] * x + [1]", -2000., 2000.);
    }

    // ヒットの位置情報をグラフに追加
    for (const auto* hit : hits) {
        const TVector3& pos = hit->GetScintillatorPosition().GetValue();
        if (hit->GetView() == B2View::kSideView) {
            hit_graph[B2View::kSideView]->SetPoint(hit_graph[B2View::kSideView]->GetN(), pos.Z(), pos.Y());
        } else if (hit->GetView() == B2View::kTopView) {
            hit_graph[B2View::kTopView]->SetPoint(hit_graph[B2View::kTopView]->GetN(), pos.Z(), pos.X());
        }
    }

    // フィッティングを行い、角度と位置を取得
    for (int iview = 0; iview < 2; iview++) {
        hit_graph[iview]->Fit(linear[iview], "Q");
        param[iview] = linear[iview]->GetParameter(0);  // tan(Y) or tan(X)
        param[iview + 2] = linear[iview]->GetParameter(1);  // pos(Y) or pos(X)
    }

    // メモリの解放
    delete linear[0];
    delete linear[1];
    delete hit_graph[0];
    delete hit_graph[1];

    return param;
}

int main(int argc, char* argv[]) {
    logging::core::get()->set_filter(logging::trivial::severity >= logging::trivial::info);

    BOOST_LOG_TRIVIAL(info) << "========== BabyMIND Position and Angle Test ==========";

    if (argc != 2) {
        BOOST_LOG_TRIVIAL(error) << "Usage: " << argv[0] << " <input B2 file path>";
        return 1;
    }

    try {
        // B2Reader を使ってB2ファイルを読み込む
        B2Reader reader(argv[1]);

        while (reader.ReadNextSpill() > 0) {
            const auto& input_spill_summary = reader.GetSpillSummary();
            
            auto it_recon_vertex = input_spill_summary.BeginReconVertex();
            while (auto* vertex = it_recon_vertex.Next()) {
                auto it_outgoing_track = vertex->BeginTrack();
                while (auto* track = it_outgoing_track.Next()) {
                    // BabyMINDトラックのみを処理
                    if (track->HasDetector(B2Detector::kBabyMind)) {
                        // トラックの初期位置と角度を計算
                        auto direction_and_position = FitBabyMind(track);

                        // 結果を表示
                        std::cout << "BabyMIND Track:" << std::endl;
                        std::cout << "  Timestamp: " << static_cast<long long int>(input_spill_summary.GetBeamSummary().GetTimestamp()) << std::endl;
                        std::cout << "  Initial Position (Y): " << direction_and_position[2] << std::endl;
                        std::cout << "  Initial Position (X): " << direction_and_position[3] << std::endl;
                        std::cout << "  Initial Angle (tan Y): " << direction_and_position[0] << std::endl;
                        std::cout << "  Initial Angle (tan X): " << direction_and_position[1] << std::endl;
                        std::cout << std::endl;
                    }
                }
            }
        }
    } catch (const std::runtime_error& error) {
        BOOST_LOG_TRIVIAL(fatal) << "Runtime error: " << error.what();
        return 1;
    } catch (const std::invalid_argument& error) {
        BOOST_LOG_TRIVIAL(fatal) << "Invalid argument error: " << error.what();
        return 1;
    }

    BOOST_LOG_TRIVIAL(info) << "========== Test Finished ==========";
    return 0;
}