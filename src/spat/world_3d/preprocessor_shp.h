// -----------------------------------------------------------------------------
/** @file preprocessorshp.h
 *  @brief interface for class bass3::PreprocessorSHP
 *  @author Bram de Greve --- BdG ---
 *  @date 2002-2003
 *  
 *  History:
 *  - 17/01/2003, BDG: creation of file
 */
// -----------------------------------------------------------------------------



/** @class bass3::PreprocessorSHP
 *  @brief A preprocessor that creates a bass3::World3 from an SHP file
 *  @author Bram de Greve --- BdG ---
 *  @date 2002-2003
 */



#ifndef BASS3_GUARDIAN_OF_INCLUSION_PREPROCESSORSHP_H
#define BASS3_GUARDIAN_OF_INCLUSION_PREPROCESSORSHP_H



// --- OLD INTERFACES ----------------------------------------------------------

#include "bass3_common.h"

#include "aabb3.h"
#include "real.h"
#include "point2.h"
#include "triangulator.h"
#include "world3.h"

#include <boost/scoped_ptr.hpp>

#include BASS3_NO_MEMORY_DEBUGGER
#include <lass/util/callback_r_1.h>
#include BASS3_MEMORY_DEBUGGER



// --- NEW INTERFACES ----------------------------------------------------------

namespace bass3
{

class BASS3_LIB_API PreprocessorSHP
{
public:

    // TYPEDEFS
    
	typedef lass::util::CallbackR1<Real, const Point2&> CBGetMapHeight;
    typedef lass::util::CallbackR1<void*, const Point2&> CBGetMapHandle;
    typedef lass::util::CallbackR1<Real, void*> CBGetHandleHeight;


    // METHODS

    void init(Triangulator* a_triangulator,
              const CBGetMapHeight& a_getMapHeight,
              const CBGetMapHandle& a_getMapHandle,
              const CBGetHandleHeight& a_getHandleHeight,
              Real a_zMin, Real a_zMax, Real a_aabbScale,
              const Vector2& a_offset);    
    World3_p process(const std::string& a_filename,
                     const std::vector<ComposedCellHandle*> a_cellHandles);

private:

    // PRIVATE DATA
    
    Triangulator* m_triangulator;
    CBGetMapHeight m_getMapHeight;
    CBGetMapHandle m_getMapHandle;
    CBGetHandleHeight m_getHandleHeight;
    Real m_zMin;
    Real m_zMax;
    Real m_aabbScale;
    Vector2 m_offset;
};



} // namespace bass3

#endif // guardian of inclusion

// --- END OF FILE preprocessorshp.h -----------------------------------------
