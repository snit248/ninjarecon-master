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

#include "BabyMindFitter.hpp"

namespace logging = boost::log;

int main(int argc, char *argv[]){
    logging::core::get()->set_filter(logging::trivial::severity >= logging::trivial::info);

    BOOST_LOG_TRIVIAL(info) << "========== BabyMIND Position Distribution Start ==========";

    if (argc != 3) {
        BOOST_LOG_TRIVIAL(error) << "Usage: " << argv[0] << " <input B2 file path>" << " <output file path>";
        return 1;
    }

    B2Reader reader(argv[1]);
    BabyMindFitter fitter;

    TH1D* h_x_pos = new TH1D("h_x_pos", "X Position Distribution", 500, -2500, 2500);
    TH1D* h_y_pos = new TH1D("h_y_pos", "Y Position Distribution", 500, -2500, 2500);
    TH1D* h_x_angle = new TH1D("h_x_angle", "X Angle Distribution", 500, -5, 5);
    TH1D* h_y_angle = new TH1D("h_y_angle", "Y Angle Distribution", 500, -5, 5);

    while (reader.ReadNextSpill() > 0) {
        // スピルサマリーを取得
        const B2SpillSummary& spill_summary = reader.GetSpillSummary();

        // 各頂点に対してトラックを処理する
        auto it_vertex = spill_summary.BeginReconVertex();
        while (const B2VertexSummary* vertex = it_vertex.Next()) {
            auto it_track = vertex->BeginTrack();
            while (const B2TrackSummary* track = it_track.Next()) {
                // BabyMINDトラックがある場合にフィットを実行
                if (track->HasDetector(B2Detector::kBabyMind)) {
                    std::vector<double> results = fitter.Fit(track);

                    // フィット結果を表示
                    std::cout << "Initial Angle (tan Y): " << results[0] << std::endl;
                    std::cout << "Initial Angle (tan X): " << results[1] << std::endl;
                    std::cout << "Initial Position (Y): " << results[2] << std::endl;
                    std::cout << "Initial Position (X): " << results[3] << std::endl;

                    h_x_pos->Fill(results[3]);
                    h_y_pos->Fill(results[2]);
                    h_x_angle->Fill(results[0]);
                    h_y_angle->Fill(results[1]);
                }
            }
        }
    }
    
    TCanvas* c_x_pos = new TCanvas("c_x_pos", "c_x_pos", 800, 600);
    TCanvas* c_y_pos = new TCanvas("c_y_pos", "c_y_pos", 800, 600);
    TCanvas* c_x_angle = new TCanvas("c_x_angle", "c_x_angle", 800, 600);
    TCanvas* c_y_angle = new TCanvas("c_y_angle", "c_y_angle", 800, 600);

    // argv[2]を出力ファイル名に含める
    std::string output_base_path = argv[2];

    // 出力ファイル名を組み立てる
    std::ostringstream x_pos_output_path;
    x_pos_output_path << output_base_path << "_x_pos_distribution.png";
    std::ostringstream y_pos_output_path;
    y_pos_output_path << output_base_path << "_y_pos_distribution.png";
    std::ostringstream x_angle_output_path;
    x_angle_output_path << output_base_path << "_x_angle_distribution.png";
    std::ostringstream y_angle_output_path;
    y_angle_output_path << output_base_path << "_y_angle_distribution.png";

    c_x_pos->cd();
    c_x_pos->SetGrid();
    h_x_pos->Draw();
    c_x_pos->SaveAs(x_pos_output_path.str().c_str());

    c_y_pos->cd();
    c_y_pos->SetGrid();
    h_y_pos->Draw();
    c_y_pos->SaveAs(y_pos_output_path.str().c_str());
    
    c_x_angle->cd();
    c_x_angle->SetGrid();
    h_x_angle->Draw();
    c_x_angle->SaveAs(x_angle_output_path.str().c_str());

    c_y_angle->cd();
    c_y_angle->SetGrid();
    h_y_angle->Draw();
    c_y_angle->SaveAs(y_angle_output_path.str().c_str());
}