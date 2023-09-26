#ifndef snap_dani_h
#define snap_dani_h
#include "snap/Snap.h"
#include <iostream>
#include <list>
#include <cstdlib>
#include <vector>
#include <string>
#include <map>
using namespace std;
class Link
{
public:
  int neighbor;
  double rank;
  int numCascades;
  Link(int N = -1, double W = -1.0, int Cas = 0)
  {
    neighbor = N;
    rank = W;
    numCascades = Cas;
  }
};
class OrginalLink
{

public:
  double rank;
  int numCascades;
  bool mark;
  OrginalLink(double W = -1.0, int Cas = 0)
  {

    rank = W;
    numCascades = Cas;
    mark = 0;
  }
};

class Orginal_Node
{
public:
  int Own;
  double sum_row;
  map<int, OrginalLink> ranklist;
  Orginal_Node()
  {
    Own = 0;
    sum_row = 0.00;
  }
};

class Help_Node
{
public:
  double sum_row;

  vector<Link> helprank;
  Help_Node()
  {
    sum_row = 0.00;
  }
};

// DANI algorithm class
class DANI
{
public:
  TVec<TCascade> CascV;
  THash<TInt, TNodeInfo> NodeNmH;
  THash<TIntPr, TEdgeInfo> EdgeInfoH;

  THash<TIntPr, TIntV> CascPerEdge; // To implement localized update
  PNGraph Graph, GroundTruth;
  TFltPrV PrecisionRecall;
  bool BoundOn, CompareGroundTruth;

  //***************Dijkstra************
  int nodes;
  Edge *weights;

  //**************Files***********
  ofstream myfile;

public:
  DANI() { BoundOn = false; }
  DANI(bool bo, bool cgt)
  {
    BoundOn = bo;
    CompareGroundTruth = cgt;
  }

  DANI(TSIn &SIn) : CascV(SIn), NodeNmH(SIn) {}
  void Save(TSOut &SOut) const
  {
    CascV.Save(SOut);
    NodeNmH.Save(SOut);
  }

  void LoadCascadesTxt(TSIn &SIn);
  void LoadGroundTruthTxt(TSIn &SIn);

  void AddGroundTruth(PNGraph &gt) { GroundTruth = gt; }
  void GenerateGroundTruth(const int &TNetwork, const int &NNodes, const int &NEdges, const TStr &NetworkParams);

  void AddCasc(const TStr &CascStr, const double &alpha = 1.0);
  void AddCasc(const TCascade &Cascade) { CascV.Add(Cascade); }
  void GenCascade(TCascade &Cascade, const int &TModel, const double &alpha, const double &beta, TIntPrIntH &EdgesUsed, const double &alpha2 = 0.0, const double &p = 1.0, const double &std_waiting_time = 0, const double &std_beta = 0);
  void GenNoisyCascade(TCascade &Cascade, const int &TModel, const double &alpha, const double &beta, TIntPrIntH &EdgesUsed,
                       const double &alpha2 = 0.0, const double &p = 1.0, const double &std_waiting_time = 0, const double &std_beta = 0, const double &PercRndNodes = 0,
                       const double &PercRndRemoval = 0);
  TCascade &GetCasc(int c) { return CascV[c]; }
  int GetCascs() { return CascV.Len(); }

  int GetNodes() { return Graph->GetNodes(); }
  // int GetNodes() { return GroundTruth->GetNodes(); }
  void AddNodeNm(const int &NId, const TNodeInfo &Info) { NodeNmH.AddDat(NId, Info); }
  TStr GetNodeNm(const int &NId) const { return NodeNmH.GetDat(NId).Name; }
  TNodeInfo GetNodeInfo(const int &NId) const { return NodeNmH.GetDat(NId); }
  bool IsNodeNm(const int &NId) const { return NodeNmH.IsKey(NId); }

  void Init();
  double GetAllCascProb(const int &EdgeN1, const int &EdgeN2);

  void SavePajek(const TStr &OutFNm);
  void SavePlaneTextNet(const TStr &OutFNm);
  void SaveEdgeInfo(const TStr &OutFNm);
  void SaveObjInfo(const TStr &OutFNm);

  void SaveGroundTruth(const TStr &OutFNm);
  void SaveCascades(const TStr &OutFNm);
  //*****************************

  void runDANI();
};

#endif
