#ifndef __ABSTRACT_DOMAINS_HH__
#define __ABSTRACT_DOMAINS_HH__

#include "llvm/Support/ErrorHandling.h"

#include "crab_llvm/config.h"
#include "crab_llvm/CfgBuilder.hh"

#include "crab/domains/linear_constraints.hpp"                     
#include "crab/domains/intervals.hpp"                      
#include "crab/domains/dis_intervals.hpp"                      
#include "crab/domains/numerical_with_congruences.hpp"
#include "crab/domains/dbm.hpp"
#include "crab/domains/array_smashing.hpp"
#include "crab/domains/term_equiv.hpp"
#include "crab/domains/boxes.hpp"

/*
   Definition of the abstract domains and a generic wrapper class (for
   crab-llvm clients) to contain an arbitrary abstract domain.  
*/


namespace crab_llvm {

  ////
  // Base (non-array) numerical domains for user options
  ////
  enum CrabDomain { INTERVALS, 
                    INTERVALS_CONGRUENCES, 
                    BOXES,
                    DIS_INTERVALS,
                    ZONES, 
                    TERMS_INTERVALS,
                    TERMS_DIS_INTERVALS,
                    NUM};

  //////
  /// Definition of the abstract domains
  //////

  using namespace crab::cfg_impl;
  using namespace crab::domains;
  using namespace ikos;

  /// --- Types for linear constraints and expressions
  typedef ikos::linear_expression<z_number, varname_t> z_lin_exp_t;
  typedef ikos::linear_constraint<z_number, varname_t> z_lin_cst_t;
  typedef ikos::linear_constraint_system<z_number, varname_t> z_lin_cst_sys_t;

  //////
  //// Base domains
  //////

  /// -- Intervals
  typedef interval_domain< z_number, varname_t> interval_domain_t;
  /// -- Zones
  typedef DBM<z_number, varname_t> dbm_domain_t;
  /// -- Boxes
  typedef boxes_domain<z_number, varname_t> boxes_domain_t;
  /// -- DisIntervals
  typedef dis_interval_domain <z_number, varname_t> dis_interval_domain_t;

  //////
  /// Combination of domains
  //////

  /// -- Reduced product of intervals with congruences
  typedef numerical_congruence_domain<interval_domain_t> ric_domain_t;
  /// -- Terms functor domain with Intervals
  typedef crab::cfg::var_factory_impl::StrVarAlloc_col::varname_t str_varname_t;
  typedef interval_domain<z_number, str_varname_t> str_interval_dom_t;
  typedef term::TDomInfo<z_number, varname_t, str_interval_dom_t> idom_info;
  typedef anti_unif<idom_info>::anti_unif_t term_int_domain_t;  
  /// -- Terms functor domain with DisIntervals
  typedef dis_interval_domain<z_number, str_varname_t> str_dis_interval_dom_t;
  typedef term::TDomInfo<z_number, varname_t, str_dis_interval_dom_t> dis_idom_info;
  typedef anti_unif<dis_idom_info>::anti_unif_t term_dis_int_domain_t;  
  /// -- Array smashing functor domain with arbitrary domain
  typedef array_smashing<interval_domain_t> arr_interval_domain_t;
  typedef array_smashing<ric_domain_t> arr_ric_domain_t;
  typedef array_smashing<dbm_domain_t> arr_dbm_domain_t;
  typedef array_smashing<term_int_domain_t> arr_term_int_domain_t;
  typedef array_smashing<term_dis_int_domain_t> arr_term_dis_int_domain_t;
  typedef array_smashing<boxes_domain_t> arr_boxes_domain_t;
  typedef array_smashing<dis_interval_domain_t> arr_dis_interval_domain_t;
}

namespace llvm {

  using namespace std;

  inline llvm::raw_ostream& operator<< (llvm::raw_ostream& o, 
                                        crab_llvm::z_lin_exp_t& e) {
    ostringstream s;
    s << e;
    o << s.str ();
    return o;
  }


  inline llvm::raw_ostream& operator<< (llvm::raw_ostream& o, 
                                        crab_llvm::z_lin_cst_t& cst) {
    ostringstream s;
    s << cst;
    o << s.str ();
    return o;
  }

  inline llvm::raw_ostream& operator<< (llvm::raw_ostream& o, 
                                        crab_llvm::z_lin_cst_sys_t& csts) {
    ostringstream s;
    s << csts;
    o << s.str ();
    return o;
  }

  inline llvm::raw_ostream& operator<< (llvm::raw_ostream& o, 
                                        crab_llvm::interval_domain_t& inv) {
    ostringstream s;
    s << inv;
    o << s.str ();
    return o;
  }
  
  inline llvm::raw_ostream& operator<< (llvm::raw_ostream& o, 
                                        crab_llvm::ric_domain_t& inv) {
    ostringstream s;
    s << inv;
    o << s.str ();
    return o;
  }

  inline llvm::raw_ostream& operator<< (llvm::raw_ostream& o, 
                                        crab_llvm::dbm_domain_t& inv) {
    ostringstream s;
    s << inv;
    o << s.str ();
    return o;
  }

  template <typename DomInfo>
  inline llvm::raw_ostream& operator<< (llvm::raw_ostream& o, 
                                        crab::domains::anti_unif<DomInfo>& inv) {
    ostringstream s;
    s << inv;
    o << s.str ();
    return o;
  }

  inline llvm::raw_ostream& operator<< (llvm::raw_ostream& o, 
                                        crab_llvm::boxes_domain_t& inv) {
    ostringstream s;
    s << inv;
    o << s.str ();
    return o;
  }

  inline llvm::raw_ostream& operator<< (llvm::raw_ostream& o, 
                                        crab_llvm::dis_interval_domain_t& inv) {
    ostringstream s;
    s << inv;
    o << s.str ();
    return o;
  }

  template <typename Base>
  inline llvm::raw_ostream& operator<< (llvm::raw_ostream& o, 
                                        crab::domains::array_smashing <Base> & inv) {
    ostringstream s;
    s << inv;
    o << s.str ();
    return o;
  }

} // end namespace llvm


namespace crab_llvm {

   //////
   /// Definition of macros
   //////

   #define DEFINE_BASE_DOMAIN(WRAPPER,ABS_DOM,ID)                                 \
   class WRAPPER: public GenericAbsDomWrapper {                                   \
     id_t m_id;                                                                   \
     ABS_DOM m_abs;                                                               \
    public:                                                                       \
     id_t getId () const { return m_id;}                                          \
     WRAPPER (ABS_DOM abs): GenericAbsDomWrapper (), m_id (ID), m_abs (abs) { }   \
     ABS_DOM get () const { return m_abs; }                                       \
     virtual z_lin_cst_sys_t to_linear_constraints () const {                     \
       ABS_DOM res (m_abs);                                                       \
       return res.to_linear_constraint_system ();}                                \
     virtual void write (std::ostream& o) const {                                 \
       ABS_DOM res (m_abs);                                                       \
       res.write (o); }                                                           \
   };                                                                             \
   template <> inline GenericAbsDomWrapperPtr                                     \
   mkGenericAbsDomWrapper (ABS_DOM abs_dom) {                                     \
     GenericAbsDomWrapperPtr res (new WRAPPER(abs_dom));                          \
     return res; }                                                                \
   template <>                                                                    \
   inline void getAbsDomWrappee (GenericAbsDomWrapperPtr wrapper,                 \
                                 ABS_DOM &abs_dom) {                              \
     auto wrappee = boost::static_pointer_cast<WRAPPER> (wrapper);                \
     abs_dom = wrappee->get (); }                                                 

   #define REGISTER_DOMAIN_ID(ABS_DOMAIN,ID)                  \
   template<>                                                 \
   inline GenericAbsDomWrapper::id_t getAbsDomId(ABS_DOMAIN)  \
   {return GenericAbsDomWrapper::ID;}                        

   #define FORGET(ABS_DOMAIN)                                    \
   do {                                                          \
     ABS_DOMAIN inv;                                             \
     getAbsDomWrappee (wrapper, inv);                            \
     crab::domain_traits::forget (inv, vs.begin (), vs.end ());  \
     return mkGenericAbsDomWrapper (inv);                        \
   } while (0) ;;


  //////
  // Generic wrapper to encapsulate an arbitrary abstract domain
  //////

  struct GenericAbsDomWrapper {

     typedef enum { intv, 
                    dbm, 
                    term_intv, 
                    term_dis_intv, 
                    ric, 
                    boxes, 
                    dis_intv,
                    arr_intv, 
                    arr_dbm, 
                    arr_term_intv, 
                    arr_term_dis_intv, 
                    arr_ric, 
                    arr_boxes,
                    arr_dis_intv} id_t;

     GenericAbsDomWrapper () { }

     virtual id_t getId () const = 0;
     virtual void write (std::ostream& o) const = 0;
     virtual z_lin_cst_sys_t to_linear_constraints () const = 0;
   };

   typedef boost::shared_ptr<GenericAbsDomWrapper> GenericAbsDomWrapperPtr;

   inline std::ostream& operator<<(std::ostream& o , 
                                   const GenericAbsDomWrapperPtr& v) {
     v->write (o);
     return o;
   }

   inline llvm::raw_ostream& operator<<(llvm::raw_ostream& o , 
                                        const GenericAbsDomWrapperPtr& v) {
     ostringstream s;
     v->write (s);
     o << s.str ();
     return o;
   }

   template <typename T> 
   inline GenericAbsDomWrapperPtr mkGenericAbsDomWrapper (T abs_dom);

   template <typename T> 
   inline void getAbsDomWrappee (GenericAbsDomWrapperPtr wrapper, T& wrappee);

   template<typename T> 
   inline GenericAbsDomWrapper::id_t getAbsDomId (T inv);

   DEFINE_BASE_DOMAIN(IntervalDomainWrapper,interval_domain_t,intv)
   DEFINE_BASE_DOMAIN(RicDomainWrapper,ric_domain_t,ric)
   DEFINE_BASE_DOMAIN(DbmDomainWrapper,dbm_domain_t,dbm)
   DEFINE_BASE_DOMAIN(TermIntDomainWrapper,term_int_domain_t,term_intv)
   DEFINE_BASE_DOMAIN(TermDisIntDomainWrapper,term_dis_int_domain_t,term_dis_intv)
   DEFINE_BASE_DOMAIN(BoxesDomainWrapper,boxes_domain_t,boxes)
   DEFINE_BASE_DOMAIN(DisIntervalDomainWrapper,dis_interval_domain_t,dis_intv)

   // Required only for array versions
   REGISTER_DOMAIN_ID(arr_interval_domain_t,arr_intv)
   REGISTER_DOMAIN_ID(arr_ric_domain_t,arr_ric)
   REGISTER_DOMAIN_ID(arr_dbm_domain_t,arr_dbm)
   REGISTER_DOMAIN_ID(arr_term_int_domain_t,arr_term_intv)
   REGISTER_DOMAIN_ID(arr_term_dis_int_domain_t,arr_term_dis_intv)
   REGISTER_DOMAIN_ID(arr_boxes_domain_t,arr_boxes)
   REGISTER_DOMAIN_ID(arr_dis_interval_domain_t,arr_dis_intv)

   template<typename B>
   class ArraySmashingDomainWrapper: public GenericAbsDomWrapper {
    public:
     
     typedef typename B::number_t N;
     typedef typename B::varname_t V;
     
    private:
     
     typedef array_smashing<B> array_smashing_t;       

     id_t m_id;     
     array_smashing_t m_abs;
     
    public:        

     id_t getId () const { return m_id;}
     
     ArraySmashingDomainWrapper(array_smashing_t abs):  
         GenericAbsDomWrapper (), m_id (getAbsDomId(abs)), m_abs (abs) { }
     
     array_smashing_t get () const {
       return m_abs;
     }
     
     z_lin_cst_sys_t to_linear_constraints () const {
       array_smashing_t res (m_abs);
       return res.to_linear_constraint_system ();
     }
     
     void write (std::ostream& o) const { 
       array_smashing_t res (m_abs);
       res.write (o);
     }
   };
      
   template <typename B> inline GenericAbsDomWrapperPtr
   mkGenericAbsDomWrapper (array_smashing<B> abs_dom) {
     GenericAbsDomWrapperPtr res (new ArraySmashingDomainWrapper<B> (abs_dom));        
     return res;
   }

   template <typename B> 
   inline void getAbsDomWrappee (GenericAbsDomWrapperPtr wrapper, 
                                 array_smashing<B>&abs_dom) {
     auto wrappee = boost::static_pointer_cast<ArraySmashingDomainWrapper<B> > (wrapper);
     abs_dom = wrappee->get ();
   }

   template<typename Range>
   inline GenericAbsDomWrapperPtr 
   forget (GenericAbsDomWrapperPtr wrapper, Range vs) {
     switch (wrapper->getId ()) {
       case GenericAbsDomWrapper::intv: FORGET(interval_domain_t)
       case GenericAbsDomWrapper::ric: FORGET(ric_domain_t)
       case GenericAbsDomWrapper::dbm: FORGET(dbm_domain_t)
       case GenericAbsDomWrapper::term_intv: FORGET(term_int_domain_t)
       case GenericAbsDomWrapper::term_dis_intv: FORGET(term_dis_int_domain_t)
       case GenericAbsDomWrapper::boxes: FORGET(boxes_domain_t)
       case GenericAbsDomWrapper::dis_intv: FORGET(dis_interval_domain_t)
       case GenericAbsDomWrapper::arr_intv: FORGET(arr_interval_domain_t)
       case GenericAbsDomWrapper::arr_ric: FORGET(arr_ric_domain_t) 
       case GenericAbsDomWrapper::arr_dbm: FORGET(arr_dbm_domain_t) 
       case GenericAbsDomWrapper::arr_term_intv: FORGET(arr_term_int_domain_t) 
       case GenericAbsDomWrapper::arr_term_dis_intv: FORGET(arr_term_dis_int_domain_t) 
       case GenericAbsDomWrapper::arr_boxes: FORGET(arr_boxes_domain_t) 
       case GenericAbsDomWrapper::arr_dis_intv: FORGET(arr_dis_interval_domain_t) 
       default: llvm_unreachable("unreachable");
     }
   }

} // end namespace
#endif
