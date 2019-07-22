
#ifndef HIT_T_HH_
#define HIT_T_HH_ 1



// LumiHit structure
typedef struct {
  int    cellID;
  double eHit;
  double xCell;
  double yCell;
  double zCell;
  double xHit;
  double yHit;
  double zHit;
  double TOF;
}Hit_t; 


#endif /* HIT_T_HH_ */
