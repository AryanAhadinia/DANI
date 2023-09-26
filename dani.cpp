#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <list>
#include <cstdlib>
#include <vector>
#include <map>
#include <string>
using namespace std;

//************************************************************

bool operator>(const Edge &e1, const Edge &e2)
{
	return e1.value > e2.value;
}

bool operator<=(const Edge &e1, const Edge &e2)
{
	return e1.value <= e2.value;
}

bool operator>=(const Edge &e1, const Edge &e2)
{
	return e1.value >= e2.value;
}

bool operator<(const Edge &e1, const Edge &e2)
{
	return e1.value < e2.value;
}

bool edgeCompare(Edge e1, Edge e2)
{
	return e1.value < e2.value;
}

void DANI::runDANI()
{

	cout << "Initialization" << endl;

	cout << "Number of Nodes : " << nodes << endl;
	vector<Orginal_Node> Total(nodes);

	double rankCoef1 = 10.0;
	double rankCoef2 = 10.0;
	int potential_edges = 0;

	cout << "Running DANI..." << endl;

	int src, dst;
	map<int, OrginalLink>::iterator it;

	for (int c = 0; c < CascV.Len(); c++)
	{

		vector<Help_Node> CascadeVector(nodes);

		for (int i = CascV[c].Len() - 1; i >= 0; i--)
		{

			Total[CascV[c].GetNode(i)].Own++;
			src = CascV[c].GetNode(i);

			for (int j = 0; j < i; j++)
			{
				dst = CascV[c].GetNode(j);
				if (CascV[c].GetTm(src) < CascV[c].GetTm(dst))
				{
					double rankValue = rankCoef1 / ((double)(CascV[c].Len() - 1 - j) * (i - j));
					CascadeVector[src].helprank.push_back(Link(dst, rankValue, 1));

					potential_edges++;

					CascadeVector[src].sum_row += rankValue;
				}
			}
		}
		// Test **************

		/*for (int i = 0; i < nodes; i++) {
			if(CascadeVector[i].helprank.size()>0){
				for(int j=0;j<CascadeVector[i].helprank.size();j++){
			if(CascadeVector[i].helprank[j].rank>0){
				if(i==0 && CascadeVector[i].helprank[j].neighbor==1){
					cout<<i<<" "<<CascadeVector[i].helprank[j].neighbor<<" "<<CascadeVector[i].helprank[j].rank<<" "<<CascadeVector[i].sum_row <<endl;}
			}
				}}}*/

		//*************

		for (int k = 0; k < Total.size(); k++)
		{
			if (CascadeVector[k].helprank.size() > 0)
			{
				for (int y = 0; y < CascadeVector[k].helprank.size(); y++)
				{
					if (CascadeVector[k].helprank[y].rank > 0)
					{
						CascadeVector[k].helprank[y].rank = (double)(CascadeVector[k].helprank[y].rank / CascadeVector[k].sum_row);
						int temp = 0;
						it = Total[k].ranklist.find(CascadeVector[k].helprank[y].neighbor);

						if (it != Total[k].ranklist.end())
						{

							it->second.numCascades += CascadeVector[k].helprank[y].numCascades;
							it->second.rank += (CascadeVector[k].helprank[y].rank);
							Total[k].sum_row += (CascadeVector[k].helprank[y].rank);
						}

						else
						{
							Total[k].ranklist.insert(pair<int, OrginalLink>(CascadeVector[k].helprank[y].neighbor, OrginalLink(CascadeVector[k].helprank[y].rank, CascadeVector[k].helprank[y].numCascades)));

							Total[k].sum_row += (CascadeVector[k].helprank[y].rank);
						}
					}
				}
			}
		}
		CascadeVector.clear();
		CascadeVector.shrink_to_fit();

		// cout << c << " is done!" << endl;
	}

	// Test **************
	/*map<int, OrginalLink>::iterator itertest;
	for (int i = 0; i < Total.size(); i++) {
		double rankI= Total[i].sum_row;
				for (itertest=Total[i].ranklist.begin(); itertest!=Total[i].ranklist.end();++itertest) {
					   cout<<i<<" "<<itertest->first<<" "<<(itertest->second.rank/ rankI)<<endl;
				}}*/
	//*********************

	weights = new Edge[potential_edges];

	cout << "potential edges : " << potential_edges << endl;

	int count = 0;

	map<int, OrginalLink>::iterator iter;

	for (int i = 0; i < Total.size(); i++)
	{

		double rankI = Total[i].sum_row;
		for (iter = Total[i].ranklist.begin(); iter != Total[i].ranklist.end(); ++iter)
		{

			weights[count].start = i;

			weights[count].end = iter->first;

			double Subscription = iter->second.numCascades;
			double Union = Total[i].Own + Total[iter->first].Own - iter->second.numCascades;
			weights[count].value = ((pow((iter->second.rank / rankI), rankCoef2)) * (pow((Subscription / Union), 2)));

			if (Subscription == Union)
			{
				weights[count].value = DBL_MAX;
			}

			count++;
		}
	}

	sort(weights, weights + count, edgeCompare);

	cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX Potential: " << potential_edges << "  X  Count: " << count << endl;
	for (int i = count - 1; i >= 0; i--)
	{
		myfile << weights[i].start;
		myfile << "\t";
		myfile << weights[i].end;
		myfile << "\t";
		myfile << weights[i].value;
		myfile << "\n";
	}
	myfile.close();
}

//********************************************************************************
//********************************************************************************

void DANI::LoadCascadesTxt(TSIn &SIn)
{
	nodes = 0;
	TStr Line;
	SIn.GetNextLn(Line);
	while (!SIn.Eof() && Line != "")
	{
		TStrV NIdV;
		Line.SplitOnAllCh(',', NIdV);
		AddNodeNm(NIdV[0].GetInt(), TNodeInfo(NIdV[1], 0));
		nodes++;
		SIn.GetNextLn(Line);
	}

	printf("All nodes read!\n");
	// int man=0;
	while (!SIn.Eof())
	{
		SIn.GetNextLn(Line);
		// man++;
		AddCasc(Line);
		// cout<<"Cascade\t"<<man<<"\n";
	}
	printf("All cascades read!\n");
}

void DANI::LoadGroundTruthTxt(TSIn &SIn)
{
	GroundTruth = TNGraph::New();
	TStr Line;

	// add nodes
	SIn.GetNextLn(Line);
	while (!SIn.Eof() && Line != "")
	{
		TStrV NIdV;
		Line.SplitOnAllCh(',', NIdV);
		GroundTruth->AddNode(NIdV[0].GetInt());
		SIn.GetNextLn(Line);
	}

	// add edges
	while (!SIn.Eof())
	{
		SIn.GetNextLn(Line);

		TStrV NIdV;
		Line.SplitOnAllCh(',', NIdV);
		GroundTruth->AddEdge(NIdV[0].GetInt(), NIdV[1].GetInt());
	}

	printf("groundtruth nodes:%d edges:%d\n", GroundTruth->GetNodes(),
		   GroundTruth->GetEdges());
}

void DANI::GenerateGroundTruth(const int &TNetwork, const int &NNodes,
							   const int &NEdges, const TStr &NetworkParams)
{
	TKronMtx SeedMtx;
	TStr MtxNm;

	switch (TNetwork)
	{
	// 2-dimension kronecker network
	case 0:
		SeedMtx = TKronMtx::GetMtx(NetworkParams.CStr()); // 0.5,0.5,0.5,0.5

		printf("\n*** Seed matrix:\n");
		SeedMtx.Dump();

		GroundTruth = TKronMtx::GenFastKronecker(SeedMtx, (int)TMath::Log2(NNodes), NEdges, true, 0);

		break;

		// forest fire network
	case 1:
		printf("Forest Fire graph for Ground Truth\n");
		TStrV NetworkParamsV;
		NetworkParams.SplitOnAllCh(';', NetworkParamsV);

		TFfGGen FF(false,						// BurnExpFireP
				   NetworkParamsV[0].GetInt(),	// StartNNodes (1)
				   NetworkParamsV[1].GetFlt(),	// ForwBurnProb (0.2)
				   NetworkParamsV[2].GetFlt(),	// BackBurnProb (0.17)
				   NetworkParamsV[3].GetInt(),	// DecayProb (1)
				   NetworkParamsV[4].GetInt(),	// Take2AmbasPrb (0)
				   NetworkParamsV[5].GetInt()); // OrphanPrb (0)

		FF.GenGraph(NNodes, false);
		GroundTruth = FF.GetGraph();

		break;
	}
}

void DANI::AddCasc(const TStr &CascStr, const double &alpha)
{
	TStrV NIdV;
	CascStr.SplitOnAllCh(';', NIdV);
	TCascade C;
	for (int i = 0; i < NIdV.Len(); i++)
	{
		TStr NId, Tm;
		NIdV[i].SplitOnCh(NId, ',', Tm);
		IAssert(IsNodeNm(NId.GetInt()));
		GetNodeInfo(NId.GetInt()).Vol = GetNodeInfo(NId.GetInt()).Vol + 1;
		C.Add(NId.GetInt(), Tm.GetFlt());
	}
	C.Sort();
	CascV.Add(C);
}

void DANI::GenCascade(TCascade &C, const int &TModel, const double &alpha,
					  const double &beta, TIntPrIntH &EdgesUsed, const double &alpha2,
					  const double &p, const double &std_waiting_time, const double &std_beta)
{
	TIntFltH InfectedNIdH;
	TIntH InfectedBy;
	double GlobalTime;
	int StartNId;

	if (GroundTruth->GetNodes() == 0)
		return;

	while (C.Len() < 2)
	{
		C.Clr();
		InfectedNIdH.Clr();
		InfectedBy.Clr();
		GlobalTime = 0;

		StartNId = GroundTruth->GetRndNId();
		InfectedNIdH.AddDat(StartNId) = GlobalTime;

		while (true)
		{
			// sort by time & get the oldest node that did not run infection
			InfectedNIdH.SortByDat(true);
			const int &NId = InfectedNIdH.BegI().GetKey();
			GlobalTime = InfectedNIdH.BegI().GetDat();

			// all the nodes has run infection
			if (GlobalTime == 1e12)
				break;

			// add current oldest node to the network and set its time
			C.Add(NId, GlobalTime);

			// run infection from the current oldest node
			const TNGraph::TNodeI NI = GroundTruth->GetNI(NId);
			for (int e = 0; e < NI.GetOutDeg(); e++)
			{
				// flip biased coin (set by beta)
				if (TInt::Rnd.GetUniDev() > beta + std_beta * TFlt::Rnd.GetNrmDev())
					continue;

				const int DstNId = NI.GetOutNId(e);

				// not infecting the parent
				if (InfectedBy.IsKey(NId) && InfectedBy.GetDat(NId).Val == DstNId)
					continue;

				double sigmaT;
				switch (TModel)
				{
				case 0:
					// exponential with alpha parameter
					sigmaT = (p * alpha + (1 - p) * alpha2) * TInt::Rnd.GetExpDev();
					while (!(sigmaT >= 1 && sigmaT < 100))
					{
						sigmaT = (p * alpha + (1 - p) * alpha2) * TInt::Rnd.GetExpDev();
					}
					break;
					// power-law
				case 1:
					// power-law with alpha parameter
					sigmaT = sigmaT = p * TInt::Rnd.GetPowerDev(alpha) + (1 - p) * TInt::Rnd.GetPowerDev(alpha2);
					while (!(sigmaT >= 1 && sigmaT < 100))
					{
						sigmaT = p * TInt::Rnd.GetPowerDev(alpha) + (1 - p) * TInt::Rnd.GetPowerDev(alpha2);
					} // skip too large time difference (>100 days)
					break;
				default:
					sigmaT = 1;
					break;
				}

				if (std_waiting_time > 0)
					sigmaT = TFlt::GetMx(0.0, sigmaT + std_waiting_time * TFlt::Rnd.GetNrmDev());

				double t1 = GlobalTime + sigmaT;

				if (InfectedNIdH.IsKey(DstNId))
				{
					double t2 = InfectedNIdH.GetDat(DstNId);
					if (t2 > t1 && t2 != 1e12)
					{
						InfectedNIdH.GetDat(DstNId) = t1;
						InfectedBy.GetDat(DstNId) = NId;
					}
				}
				else
				{
					InfectedNIdH.AddDat(DstNId) = t1;
					InfectedBy.AddDat(DstNId) = NId;
				}
			}

			// we cannot delete key (otherwise, we cannot sort), so we assign a very big time
			InfectedNIdH.GetDat(NId) = 1e12;
		}
	}

	C.Sort();

	for (TIntH::TIter EI = InfectedBy.BegI(); EI < InfectedBy.EndI(); EI++)
	{
		TIntPr Edge(EI.GetDat().Val, EI.GetKey().Val);

		if (!EdgesUsed.IsKey(Edge))
			EdgesUsed.AddDat(Edge) = 0;

		EdgesUsed.GetDat(Edge) += 1;
	}
}

void DANI::GenNoisyCascade(TCascade &C, const int &TModel, const double &alpha,
						   const double &beta, TIntPrIntH &EdgesUsed, const double &alpha2,
						   const double &p, const double &std_waiting_time,
						   const double &std_beta, const double &PercRndNodes,
						   const double &PercRndRemoval)
{
	TIntPrIntH EdgesUsedC; // list of used edges for a single cascade
	GenCascade(C, TModel, alpha, beta, EdgesUsedC, alpha2, p, std_waiting_time,
			   std_beta);

	// store keys
	TIntV KeyV;
	C.NIdHitH.GetKeyV(KeyV);

	// store first and last time
	double tbeg = TFlt::Mx, tend = TFlt::Mn;
	for (int i = 0; i < KeyV.Len(); i++)
	{
		if (tbeg > C.NIdHitH.GetDat(KeyV[i]).Tm)
			tbeg = C.NIdHitH.GetDat(KeyV[i]).Tm;
		if (tend < C.NIdHitH.GetDat(KeyV[i]).Tm)
			tend = C.NIdHitH.GetDat(KeyV[i]).Tm;
	}

	// remove PercRndRemoval% of the nodes of the cascades
	if (PercRndRemoval > 0)
	{
		for (int i = KeyV.Len() - 1; i >= 0; i--)
		{
			if (TFlt::Rnd.GetUniDev() < PercRndRemoval)
			{
				// remove from the EdgesUsedC the ones affected by the removal
				TIntPrV EdgesToRemove;
				for (TIntPrIntH::TIter EI = EdgesUsedC.BegI(); EI < EdgesUsedC.EndI(); EI++)
				{
					if ((KeyV[i] == EI.GetKey().Val1 && C.IsNode(EI.GetKey().Val2) && C.GetTm(KeyV[i]) < C.GetTm(EI.GetKey().Val2)) || (KeyV[i] == EI.GetKey().Val2 && C.IsNode(EI.GetKey().Val1) && C.GetTm(KeyV[i]) > C.GetTm(EI.GetKey().Val1)))
					{
						EI.GetDat() = EI.GetDat() - 1;

						if (EI.GetDat() == 0)
							EdgesToRemove.Add(EI.GetKey());
					}
				}

				for (int er = 0; er < EdgesToRemove.Len(); er++)
					EdgesUsedC.DelKey(EdgesToRemove[er]);

				C.Del(KeyV[i]);
			}
		}

		// defrag the hash table, otherwise other functions can crash
		C.NIdHitH.Defrag();
	}

	// Substitute PercRndNodes% of the nodes for a random node at a random time
	if (PercRndNodes > 0)
	{
		for (int i = KeyV.Len() - 1; i >= 0; i--)
		{
			if (TFlt::Rnd.GetUniDev() < PercRndNodes)
			{
				// remove from the EdgesUsedC the ones affected by the change
				TIntPrV EdgesToRemove;
				for (TIntPrIntH::TIter EI = EdgesUsedC.BegI(); EI < EdgesUsedC.EndI(); EI++)
				{
					if ((KeyV[i] == EI.GetKey().Val1 && C.IsNode(EI.GetKey().Val2) && C.GetTm(KeyV[i]) < C.GetTm(EI.GetKey().Val2)) || (KeyV[i] == EI.GetKey().Val2 && C.IsNode(EI.GetKey().Val1) && C.GetTm(KeyV[i]) > C.GetTm(EI.GetKey().Val1)))
					{
						EI.GetDat() = EI.GetDat() - 1;

						if (EI.GetDat() == 0)
							EdgesToRemove.Add(EI.GetKey());
					}
				}

				for (int er = 0; er < EdgesToRemove.Len(); er++)
					EdgesUsedC.DelKey(EdgesToRemove[er]);

				printf("Old node n:%d t:%f --", KeyV[i].Val, C.GetTm(KeyV[i]));
				C.Del(KeyV[i]);

				// not repeating a label
				double tnew = 0;
				int keynew = -1;
				do
				{
					tnew = tbeg + TFlt::Rnd.GetUniDev() * (tend - tbeg);
					keynew = Graph->GetRndNId();
				} while (KeyV.IsIn(keynew));

				printf("New node n:%d t:%f\n", keynew, tnew);

				C.Add(keynew, tnew);
				KeyV.Add(keynew);
			}
		}
	}

	// add to the aggregate list (EdgesUsed)
	EdgesUsedC.Defrag();

	for (int i = 0; i < EdgesUsedC.Len(); i++)
	{
		if (!EdgesUsed.IsKey(EdgesUsedC.GetKey(i)))
			EdgesUsed.AddDat(EdgesUsedC.GetKey(i)) = 0;

		EdgesUsed.GetDat(EdgesUsedC.GetKey(i)) += 1;
	}
}

void DANI::SavePajek(const TStr &OutFNm)
{
	TIntSet NIdSet;
	FILE *F = fopen(OutFNm.CStr(), "wt");
	fprintf(F, "*Vertices %d\r\n", NIdSet.Len());
	for (THash<TInt, TNodeInfo>::TIter NI = NodeNmH.BegI(); NI < NodeNmH.EndI(); NI++)
	{
		const TNodeInfo &I = NI.GetDat();
		fprintf(F, "%d \"%s\" ic Blue x_fact %f y_fact %f\r\n",
				NI.GetKey().Val, I.Name.CStr(), TMath::Mx<double>(log((double)I.Vol) - 5, 1), TMath::Mx<double>(log((double)I.Vol) - 5, 1));
	}
	fprintf(F, "*Arcs\r\n");
	for (TNGraph::TEdgeI EI = Graph->BegEI(); EI < Graph->EndEI(); EI++)
	{
		fprintf(F, "%d %d 1\r\n", EI.GetSrcNId(), EI.GetDstNId());
	}
	fclose(F);
}

void DANI::SavePlaneTextNet(const TStr &OutFNm)
{
	TIntSet NIdSet;
	FILE *F = fopen(OutFNm.CStr(), "wt");
	for (THash<TInt, TNodeInfo>::TIter NI = NodeNmH.BegI(); NI < NodeNmH.EndI(); NI++)
	{
		const TNodeInfo &I = NI.GetDat();
		fprintf(F, "%d,%d\r\n", NI.GetKey().Val, NI.GetKey().Val);
	}

	fprintf(F, "\r\n");

	for (TNGraph::TEdgeI EI = Graph->BegEI(); EI < Graph->EndEI(); EI++)
	{
		fprintf(F, "%d,%d\r\n", EI.GetSrcNId(), EI.GetDstNId());
	}
	fclose(F);
}

void DANI::SaveEdgeInfo(const TStr &OutFNm)
{
	FILE *F = fopen(OutFNm.CStr(), "wt");

	fprintf(F, "src dst vol marginal_gain median_timediff average_timediff\n");
	for (THash<TIntPr, TEdgeInfo>::TIter EI = EdgeInfoH.BegI(); EI < EdgeInfoH.EndI(); EI++)
	{
		TEdgeInfo &EdgeInfo = EI.GetDat();
		fprintf(F, "%s/%s/%d/%f/%f/%f\n",
				NodeNmH.GetDat(EI.GetKey().Val1.Val).Name.CStr(),
				NodeNmH.GetDat(EI.GetKey().Val2.Val).Name.CStr(),
				EdgeInfo.Vol.Val, EdgeInfo.MarginalGain.Val,
				EdgeInfo.MedianTimeDiff.Val, EdgeInfo.AverageTimeDiff.Val);
	}
	fclose(F);
}

void DANI::SaveObjInfo(const TStr &OutFNm)
{
	TGnuPlot GnuPlot(OutFNm);

	TFltV Objective, Bound;

	for (THash<TIntPr, TEdgeInfo>::TIter EI = EdgeInfoH.BegI(); EI < EdgeInfoH.EndI(); EI++)
	{
		if (Objective.Len() == 0)
		{
			Bound.Add(EI.GetDat().MarginalBound + EI.GetDat().MarginalGain);
			Objective.Add(EI.GetDat().MarginalGain);
		}
		else
		{
			Objective.Add(Objective[Objective.Len() - 1] + EI.GetDat().MarginalGain);
			Bound.Add(EI.GetDat().MarginalBound + Objective[Objective.Len() - 1]);
		}
	}

	GnuPlot.AddPlot(Objective, gpwLinesPoints, "NETINF");
	GnuPlot.AddPlot(Bound, gpwLinesPoints, "Upper Bound (Th. 4)");

	GnuPlot.SavePng();
}

void DANI::SaveGroundTruth(const TStr &OutFNm)
{
	TFOut FOut(OutFNm);

	// write nodes to file
	for (TNGraph::TNodeI NI = GroundTruth->BegNI(); NI < GroundTruth->EndNI(); NI++)
	{
		FOut.PutStr(TStr::Fmt("%d,%d\r\n", NI.GetId(), NI.GetId())); // nodes
	}

	FOut.PutStr("\r\n");

	// write edges to file (not allowing self loops in the network)
	for (TNGraph::TEdgeI EI = GroundTruth->BegEI(); EI < GroundTruth->EndEI(); EI++)
	{
		// not allowing self loops in the Kronecker network
		if (EI.GetSrcNId() != EI.GetDstNId())
			FOut.PutStr(TStr::Fmt("%d,%d\r\n", EI.GetSrcNId(), EI.GetDstNId()));
	}
}

void DANI::SaveCascades(const TStr &OutFNm)
{
	TFOut FOut(OutFNm);

	// write nodes to file
	for (TNGraph::TNodeI NI = GroundTruth->BegNI(); NI < GroundTruth->EndNI(); NI++)
	{
		FOut.PutStr(TStr::Fmt("%d,%d\r\n", NI.GetId(), NI.GetId())); // nodes
	}

	// FOut.PutStr("\r\n");

	// write cascades to file
	for (int i = 0; i < CascV.Len(); i++)
	{
		TCascade &C = CascV[i];
		int j = 0;

		for (THash<TInt, THitInfo>::TIter NI = C.NIdHitH.BegI(); NI < C.NIdHitH.EndI(); NI++, j++)
		{
			if (j > 0)
				FOut.PutStr(TStr::Fmt(";%d,%f", NI.GetDat().NId.Val,
									  NI.GetDat().Tm.Val));
			else
				FOut.PutStr(TStr::Fmt("%d,%f", NI.GetDat().NId.Val,
									  NI.GetDat().Tm.Val));
		}

		if (C.Len() >= 1)
			FOut.PutStr(TStr::Fmt("\r\n"));
	}
}
