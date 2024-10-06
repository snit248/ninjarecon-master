// system includes
#include <vector>
#include <numeric>
#include <algorithm>
#include <iostream>

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

using namespace std;

int main(int argc, char *argv[]) {
    B2Reader reader(argv[1]);
    int nEntries = reader.GetEntryNumber();

    while(reader.ReadNextSpill() > 0) {
        auto &input_spill_summary = reader.GetSpillSummary();
        auto &input_beam_summary = input_spill_summary.GetBeamSummary();
        auto &input_hit_summary = input_spill_summary.GetHitSummary();

        int timestamp = input_beam_summary.GetTimestamp();
        cout<<"timestamp : "<<timestamp<<endl;
        cout<<"nEntries : "<<nEntries<<endl;

        

    }

}