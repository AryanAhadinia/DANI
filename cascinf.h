#ifndef snap_cascinf_h
#define snap_cascinf_h

#include "snap/Snap.h"

// Hit info (timestamp, candidate parent) about a node in a cascade
class THitInfo
{
public:
  TInt NId, Parent;
  TFlt Tm;

public:
  THitInfo(const int &NodeId = -1, const double &HitTime = 0) : NId(NodeId), Parent(-1), Tm(HitTime) {}
  THitInfo(TSIn &SIn) : NId(SIn), Parent(SIn), Tm(SIn) {}
  void Save(TSOut &SOut) const
  {
    NId.Save(SOut);
    Parent.Save(SOut);
    Tm.Save(SOut);
  }
  bool operator<(const THitInfo &Hit) const
  {
    return Tm < Hit.Tm;
  }
};

// Cascade
class TCascade
{
public:
  THash<TInt, THitInfo> NIdHitH;
  TFlt CurProb, Alpha;
  double Eps;

public:
  TCascade() : NIdHitH(), CurProb(0), Alpha(1.0), Eps(1e-64) {}
  TCascade(const double &alpha) : NIdHitH(), CurProb(0), Eps(1e-64) { Alpha = alpha; }
  TCascade(const double &alpha, const double &eps) : NIdHitH(), CurProb(0)
  {
    Alpha = alpha;
    Eps = eps;
  }
  TCascade(TSIn &SIn) : NIdHitH(SIn), CurProb(SIn), Alpha(SIn) {}
  void Save(TSOut &SOut) const
  {
    NIdHitH.Save(SOut);
    CurProb.Save(SOut);
    Alpha.Save(SOut);
  }
  void Clr()
  {
    NIdHitH.Clr();
    CurProb = 0;
    Alpha = 1.0;
  }
  int Len() const { return NIdHitH.Len(); }
  int GetNode(const int &i) const { return NIdHitH.GetKey(i); }
  int GetParent(const int NId) const { return NIdHitH.GetDat(NId).Parent; }
  double GetAlpha() const { return Alpha; }
  double GetTm(const int &NId) const { return NIdHitH.GetDat(NId).Tm; }
  void Add(const int &NId, const double &HitTm) { NIdHitH.AddDat(NId, THitInfo(NId, HitTm)); }
  void Del(const int &NId) { NIdHitH.DelKey(NId); }
  bool IsNode(const int &NId) const { return NIdHitH.IsKey(NId); }
  void Sort() { NIdHitH.SortByDat(false); }
  double TransProb(const int &NId1, const int &NId2) const;
  double GetProb(const PNGraph &G);
  void InitProb();
  double UpdateProb(const int &N1, const int &N2, const bool &UpdateProb = false);
};

// Node info (name and number of cascades)
class TNodeInfo
{
public:
  TStr Name;
  TInt Vol;

public:
  TNodeInfo() {}
  TNodeInfo(const TStr &NodeNm, const int &Volume) : Name(NodeNm), Vol(Volume) {}
  TNodeInfo(TSIn &SIn) : Name(SIn), Vol(SIn) {}
  void Save(TSOut &SOut) const
  {
    Name.Save(SOut);
    Vol.Save(SOut);
  }
};

// Edge info (name and number of cascades)
class TEdgeInfo
{
public:
  TInt Vol;
  TFlt MarginalGain, MarginalBound, MedianTimeDiff, AverageTimeDiff; // we can skip MarginalBound for efficiency if not explicitly required
public:
  TEdgeInfo() {}
  TEdgeInfo(const int &v,
            const double &mg,
            const double &mb,
            const double &mt,
            const double &at) : Vol(v), MarginalGain(mg), MarginalBound(mb), MedianTimeDiff(mt), AverageTimeDiff(at) {}
  TEdgeInfo(const int &v,
            const double &mg,
            const double &mt,
            const double &at) : Vol(v), MarginalGain(mg), MarginalBound(0), MedianTimeDiff(mt), AverageTimeDiff(at) {}
  TEdgeInfo(TSIn &SIn) : Vol(SIn), MarginalGain(SIn), MarginalBound(SIn), MedianTimeDiff(SIn), AverageTimeDiff(SIn) {}
  void Save(TSOut &SOut) const
  {
    Vol.Save(SOut);
    SOut.Save(MarginalGain);
    SOut.Save(MarginalBound);
    SOut.Save(MedianTimeDiff);
    SOut.Save(AverageTimeDiff);
  } //
};

class Edge
{
public:
  int start;
  int end;
  double value;
  Edge(){};
  friend bool operator<(const Edge &e1, const Edge &e2);
  friend bool operator>(const Edge &e1, const Edge &e2);
  friend bool operator>=(const Edge &e1, const Edge &e2);
  friend bool operator<=(const Edge &e1, const Edge &e2);
};

#endif
