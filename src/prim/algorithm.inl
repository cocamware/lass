namespace lass
{
namespace prim
{


template<typename T, class DegeneratePolicy>
bool triangulate(const SimplePolygon2D<T, DegenerationPolicy>& iPolygon,
				 std::vector<Triangle2D<T> >& oTriangles)
{
	oTriangles.clear();
	if (iPolygon.isConvex())
	{
		// this is the easy one :)
		int i=0;
		for (;i<iPolygon.size()-3;++i)
			oTriangles.push_back(Triangle2D<T>(iPolygon[0],iPolygon[i+1],iPolygon[i+2] ) );
		return true;
	}

	// we implement the easiest algorithm: ear clipping
	// we try to find a non-reflex vertex and then clip it

	SimplePolygon2D<T, DegenerationPolicy> temp(iPolygon);
	while (temp.size()>3)
	{
		int i=0;
		for (;i<temp.size();++i)
		{
			if (temp.isReflex(i))
			{
				oTriangles.push_back(Triangle2D<T>(temp[i-1],temp[i],temp[i+1]));
				temp.erase(i);
				break;
			}
		}
	}
	oTriangles.push_back(Triangle2D<T>(temp[0],temp[1],temp[2]));
	return true;
}

}
}
