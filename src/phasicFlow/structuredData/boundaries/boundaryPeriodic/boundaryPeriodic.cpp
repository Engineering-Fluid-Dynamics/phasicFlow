/*------------------------------- phasicFlow ---------------------------------
      O        C enter of
     O O       E ngineering and
    O   O      M ultiscale modeling of
   OOOOOOO     F luid flow       
------------------------------------------------------------------------------
  Copyright (C): www.cemf.ir
  email: hamid.r.norouzi AT gmail.com
------------------------------------------------------------------------------  
Licence:
  This file is part of phasicFlow code. It is a free software for simulating 
  granular and multiphase flows. You can redistribute it and/or modify it under
  the terms of GNU General Public License v3 or any other later versions. 
 
  phasicFlow is distributed to help others in their research in the field of 
  granular and multiphase flows, but WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

-----------------------------------------------------------------------------*/

#include "boundaryPeriodic.hpp"
#include "internalPoints.hpp"
#include "dictionary.hpp"


pFlow::boundaryPeriodic::boundaryPeriodic
(
	const dictionary &dict,
    const plane 	&bplane,
    internalPoints 	&internal,
	boundaryList	&bndrs,
	uint32 			thisIndex
)
:
	boundaryBase(dict, bplane, internal, bndrs, thisIndex),
	mirrorBoundaryIndex_(dict.getVal<uint32>("mirrorBoundaryIndex"))
{
	
}

bool pFlow::boundaryPeriodic::beforeIteration(
	uint32 step, 
	const timeInfo& ti, 
	bool updateIter, 
	bool iterBeforeUpdate , 
	bool& callAgain)
{
	if(step==1)
	{
		boundaryBase::beforeIteration(step, ti, updateIter, iterBeforeUpdate, callAgain);
		callAgain = true;
		return true;
	}

	
	callAgain = false;
	// nothing have to be done
	if(empty())
	{
		return true;
	}
	//output<<this->thisBoundaryIndex()<<"  ->"<<ti.iter()<<" update called\n";
	if(!performBoundarytUpdate())
	{
		return true;
	}

	//output<<this->thisBoundaryIndex()<<"  ->"<< ti.iter()<<" update is being performed \n";
	uint32 s = size();
	uint32Vector_D transferFlags("transferFlags",s+1, s+1, RESERVE()); 
	transferFlags.fill(0u);
	
	auto points = thisPoints();
	auto p = boundaryPlane().infPlane();
	const auto & transferD = transferFlags.deviceViewAll();
	
	uint32 numTransfered = 0;

	Kokkos::parallel_reduce
	(
		"boundaryPeriodic::beforeIteration",
		deviceRPolicyStatic(0u,s),
		LAMBDA_HD(uint32 i, uint32& trnasToUpdate)
		{
			if(p.pointInNegativeSide(points(i)))
			{
				transferD(i)=1;
				trnasToUpdate++;
			}
		}, 
		numTransfered
	);

	// no point to be transfered 
	if(numTransfered == 0 )
	{
		return true;
	}
	
	// to obtain the transfer vector 
	realx3 transferVec = displacementVectroToMirror();
	
	return transferPointsToMirror
	(
		numTransfered,
		transferFlags, 
		mirrorBoundaryIndex(), 
		transferVec
	);
	
	return true;
	
	
}

bool pFlow::boundaryPeriodic::iterate(const timeInfo& ti)
{
	return true;
}

bool pFlow::boundaryPeriodic::afterIteration(const timeInfo& ti)
{
	return true;
}