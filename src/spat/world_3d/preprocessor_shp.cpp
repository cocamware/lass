// -----------------------------------------------------------------------------
/** @file preprocessorshp.cpp
 *  @brief implementation of class bass3::PreprocessorSHP
 *  @author Bram de Greve --- BdG ---
 *  @date 2002-2003
 *
 *  History:
 *  - 17/01/2003, BDG: creation of file
 */
// -----------------------------------------------------------------------------



// --- INCLUDED INTERFACES -----------------------------------------------------

#include "bass3_common.h"
#include "preprocessorshp.h"

#include "preprocessor2d5.h"
#include "simpleshpreader.h"



namespace bass3
{



// --- PUBLIC METHODS ----------------------------------------------------------

/** (Re)Initialize the preprocessor for building a world.
 *  @param a_triangulator Same as for bass3::Preprocessor2D5::init().
 *  @param a_getMapHeight Same as for bass3::Preprocessor2D5::init().
 *  @param a_getMapHandle Same as for bass3::Preprocessor2D5::init().
 *  @param a_getHandleHeight Same as for bass3::Preprocessor2D5::init().
 *  @param a_zMin z-bound of world.
 *  @param a_zMax z-bound of world.
 *  @param a_aabbScale value to scale AABB of World3 as constructed by shape-
 *                     file min/max.  Notice that a_zMin & a_zMax will NOT
 *                     be scaled by this!
 *  @param a_offset vector that will be added to the vertices as they come out
 *                  of the shapefile.
 */
void PreprocessorSHP::init(Triangulator* a_triangulator,
						   const CBGetMapHeight& a_getMapHeight,
						   const CBGetMapHandle& a_getMapHandle,
						   const CBGetHandleHeight& a_getHandleHeight,
						   Real a_zMin, Real a_zMax, Real a_aabbScale,
						   const Vector2& a_offset)
{
	BASS3_HEADER("bass3::PreprocessorSHP::init ...");

	m_triangulator = a_triangulator;
	m_getMapHeight = a_getMapHeight;
	m_getMapHandle = a_getMapHandle;
	m_getHandleHeight = a_getHandleHeight;
	m_zMin = a_zMin;
	m_zMax = a_zMax;
	m_aabbScale = a_aabbScale;
	m_offset = a_offset;
}



/** Build a World3 from a shapefile.
 *  You can process multiple times without reinitializing nor interference
 *  between different processes.
 *  @param a_filename The filename of the shapefile.  this file must fullfill
 *                    the conditions as described above for PreprocessorSHP.
 *  @param a_handles composed cell handles.  each obstacle will refer to one
 *                   of these handles by an index.  If this index is omitted,
 *                   index = 0 is assumed.
 *
 * @todo CURRENTLY PREPROCESSORSHP CAN'T HANDLE WITH MORE THAN ONE HANDLE
 */
World3_p PreprocessorSHP::process(const std::string& a_filename,
								  std::vector<ComposedCellHandle*> a_handles)
{
	BASS3_HEADER("bass3::PreprocessorSHP::process ...");

	typedef SimpleSHPReader::TripleCoord TripleCoord;
	typedef std::vector<TripleCoord> TripleCoord_vector;

	// open shape file.
	SimpleSHPReader reader;
	bool success = reader.open(a_filename);
	if (!success)
	{
		LASS_THROW("failed to open gis::SimpleSHPReader for '" << a_filename << "'");
	}

	// construct world3 AABB.
	TripleCoord shpMin;
	TripleCoord shpMax;
	reader.getMinMax(shpMin, shpMax);
	Vector2 min = (Vector2(shpMin.x, shpMin.y) + m_offset);
	Vector2 max = (Vector2(shpMax.x, shpMax.y) + m_offset);
	AABB3 aabb(min.x, min.y, m_zMin, max.x, max.y, m_zMax);
	aabb.scale(m_aabbScale);

	// get internal preprocessor ready
	Preprocessor2D5 prep2D5;
	prep2D5.init(m_triangulator,
				 m_getMapHeight, m_getMapHandle, m_getHandleHeight,
				 aabb);

	// process data
	for (unsigned i = 0; i < reader.size(); ++i)
	{
		TripleCoord_vector shape;
		TripleCoord_vector::reverse_iterator tit;
		Polygon2 poly;

		// shapes are clockwise, first and last vertex are equal.
		// we must go to counterclockwise, first and last vertex not equal.
		reader.getRecord(i, shape);
		for (tit = shape.rbegin() + 1; tit != shape.rend(); ++tit)
		{
			const Point2 p(tit->x, tit->y);
			poly.addVertex(p + m_offset);
			LASS_ASSERT(poly.size() == 1 || p != poly.vertex(0));
		}

		// TODO: make sure you can do some more handles!
		prep2D5.addObstacle(poly, a_handles[0]);
	}

	return prep2D5.process();
}



} // namespace bass3

// --- END OF FILE preprocessorshp.cpp -----------------------------------------
