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

#include "sphereParticles.hpp"
#include "setFieldList.hpp"
#include "sphereParticlesKernels.hpp"

pFlow::uniquePtr<pFlow::List<pFlow::eventObserver*>> 
pFlow::sphereParticles::getFieldObjectList()const
{
	auto objListPtr = particles::getFieldObjectList();

	objListPtr().push_back(
		static_cast<eventObserver*>(&I_) );

	return objListPtr;
}

bool pFlow::sphereParticles::diameterMassInertiaPropId
(
	const word& shName,
	real& diam,
	real& mass,
	real& I,
	int8& propIdx
)
{
	uint32 idx;
	if( !shapes_.nameToIndex(shName, idx) )
	{
		printKeys(fatalErrorInFunction<<
		"  wrong shape name in the input: "<< shName<<endl<<
		"  available shape names are: ", shapes_.names())<<endl;
		return false;
	}

	diam = shapes_.diameter(idx);
	word materialName = shapes_.material(idx);
	uint32 pIdx;
	if( !property_.nameToIndex(materialName, pIdx) )
	{
		fatalErrorInFunction << 
		"  wrong material name "<< materialName <<" specified for shape "<< shName<<
		"  in the sphereShape dictionary.\n"<<
		"  available materials are "<< property_.materials()<<endl;
		return false;
	}
	real rho = property_.density(pIdx);

	mass = Pi/6.0*pow(diam,3.0)*rho;
	I = 0.4 * mass * pow(diam/2.0,2.0);
	propIdx= static_cast<int8>(pIdx);
	return true;
}

bool pFlow::sphereParticles::initializeParticles()
{
		
	int32IndexContainer indices(0, shapeName_.size());
	return insertSphereParticles(shapeName_, indices);
}


bool pFlow::sphereParticles::beforeIteration() 
{
	particles::beforeIteration();
	
	intPredictTimer_.start();
	
	//Info<<"before dyn predict"<<endInfo;
		dynPointStruct_.predict(this->dt(), accelertion_);
	//Info<<"after dyn predict"<<endInfo;

	//Info<<"before revel predict"<<endInfo;
		rVelIntegration_().predict(this->dt(),rVelocity_, rAcceleration_);
	//Info<<"after rvel predict"<<endInfo;

	intPredictTimer_.end();

	return true;
}


bool pFlow::sphereParticles::iterate() 
{

	accelerationTimer_.start();
	//Info<<"before accelerationTimer_ "<<endInfo;
		pFlow::sphereParticlesKernels::acceleration(
			control().g(),
			mass().deviceVectorAll(),
			contactForce().deviceVectorAll(),
			I().deviceVectorAll(),
			contactTorque().deviceVectorAll(),
			pStruct().activePointsMaskD(),
			accelertion().deviceVectorAll(),
			rAcceleration().deviceVectorAll()
			);
	accelerationTimer_.end();
	
	intCorrectTimer_.start();
	//Info<<"before correct dyn "<<endInfo;
		dynPointStruct_.correct(this->dt(), accelertion_);
	//Info<<"after correct dyn "<<endInfo;
		rVelIntegration_().correct(this->dt(), rVelocity_, rAcceleration_);
	//Info<<"after correct rvel "<<endInfo;
	intCorrectTimer_.end();
	
	return true;
}


bool pFlow::sphereParticles::afterIteration() 
{
	return true;
}


bool pFlow::sphereParticles::insertSphereParticles(
	const wordVector& names,
	const int32IndexContainer& indices
	)
{

	if(names.size()!= indices.size())
	{
		fatalErrorInFunction <<
		"sizes of names ("<<names.size()<<") and indices ("
			<< indices.size()<<") do not match \n";
		return false;
	}

	auto len = names.size();
	
	realVector  diamVec(len, RESERVE());
	realVector  massVec(len, RESERVE());
	realVector  IVec(len, RESERVE());
	int8Vector  pIdVec(len, RESERVE());
	int32Vector IdVec(len, RESERVE());

	real d, m, I;
	int8 pId;

	forAll(i, names )
	{
		if (diameterMassInertiaPropId(names[i], d, m, I, pId))
		{
			diamVec.push_back(d);
			massVec.push_back(m);
			IVec.push_back(I);
			pIdVec.push_back(pId);
			IdVec.push_back(idHandler_.getNextId());
		}
		else
		{
			fatalErrorInFunction<< "failed to calculate properties of shape " <<
			 names[i]<<endl;
			return false;
		}
		
	}

	if(!diameter_.insertSetElement(indices, diamVec))
	{
		fatalErrorInFunction<< " failed to insert diameters to the diameter field. \n";
		return false;
	}

	if(!mass_.insertSetElement(indices, massVec))
	{
		fatalErrorInFunction<< " failed to insert mass to the mass field. \n";
		return false;
	}

	if(!I_.insertSetElement(indices, IVec))
	{
		fatalErrorInFunction<< " failed to insert I to the I field. \n";
		return false;
	}

	if(!propertyId_.insertSetElement(indices, pIdVec))
	{
		fatalErrorInFunction<< " failed to insert propertyId to the propertyId field. \n";
		return false;
	}

	if(!id_.insertSetElement(indices, IdVec))
	{
		fatalErrorInFunction<< " failed to insert id to the id field. \n";
		return false;
	}

	return true;

}


pFlow::sphereParticles::sphereParticles(
	systemControl &control,
	const property& prop
)
:
	particles(
		control,
		control.settingsDict().getValOrSet(
			"integrationMethod",
			word("AdamsBashforth3")
			)
		),
	property_(prop),
	shapes_(
		control.caseSetup().emplaceObjectOrGet<sphereShape>(
			objectFile(
				sphereShapeFile__,
				"",
				objectFile::READ_ALWAYS,
				objectFile::WRITE_NEVER
				)
			)
		),
	I_(
		this->time().emplaceObject<realPointField_D>(
			objectFile(
				"I",
				"",
				objectFile::READ_NEVER,
				objectFile::WRITE_ALWAYS
				),
			pStruct(),
			static_cast<real>(0.0000000001)
			)
		),
	rVelocity_(
		this->time().emplaceObject<realx3PointField_D>(
			objectFile(
				"rVelocity",
				"",
				objectFile::READ_IF_PRESENT,
				objectFile::WRITE_ALWAYS
				),
			pStruct(),
			zero3
			)
		),
	rAcceleration_(
		this->time().emplaceObject<realx3PointField_D>(
			objectFile(
				"rAcceleration",
				"",
				objectFile::READ_IF_PRESENT,
				objectFile::WRITE_ALWAYS
				),
			pStruct(),
			zero3
			)
		),
	accelerationTimer_(
		"Acceleration", &this->timers() ),
	intPredictTimer_(
		"Integration-predict", &this->timers() ),
	intCorrectTimer_(
		"Integration-correct", &this->timers() )

{

	Report(1)<<"Creating integration method "<<greenText(this->integrationMethod())
		  << " for rotational velocity."<<endReport;
		  
	rVelIntegration_ = 
		integration::create(
			"rVelocity",
			this->time().integration(),
			this->pStruct(),
			this->integrationMethod());

	if( !rVelIntegration_ )
	{
		fatalErrorInFunction<<
		"  error in creating integration object for rVelocity. \n";
		fatalExit;
	}

	
	if(rVelIntegration_->needSetInitialVals())
	{
		
		auto [minInd, maxInd] = pStruct().activeRange();
		int32IndexContainer indexHD(minInd, maxInd);
	
		auto n = indexHD.size();
		auto index = indexHD.indicesHost();

		realx3Vector rvel(n,RESERVE());
		const auto hrVel = rVelocity_.hostVector();

		for(auto i=0; i<n; i++)
		{
			rvel.push_back( hrVel[index(i)]);
		}
		
		Report(2)<< "Initializing the required vectors for rotational velocity integratoin\n "<<endReport;
		rVelIntegration_->setInitialVals(indexHD, rvel);

	}

	if(!initializeParticles())
	{
		fatalExit;
	}
	
} 

bool pFlow::sphereParticles::update(const eventMessage& msg) 
{
	
	if(rVelIntegration_->needSetInitialVals())
	{
		
		
		auto indexHD = pStruct().insertedPointIndex();
	
		auto n = indexHD.size();
		auto index = indexHD.indicesHost();

		realx3Vector rvel(n,RESERVE());
		const auto hrVel = rVelocity_.hostVector();

		for(auto i=0; i<n; i++)
		{
			rvel.push_back( hrVel[index(i)]);
		}
		
		rVelIntegration_->setInitialVals(indexHD, rvel);

	}	

	return true;
}

bool pFlow::sphereParticles::insertParticles
(
	const realx3Vector& position,
 	const wordVector&  shapes,
 	const setFieldList& setField
 )
{
	
	if( position.size() != shapes.size() )
	{
		fatalErrorInFunction<< 
		"  size of vectors position ("<<position.size()<<
		") and shapes ("<<shapes.size()<<") are not the same. \n";
		return false;
	}

	auto exclusionListAllPtr = getFieldObjectList(); 

	auto newInsertedPtr = pStruct().insertPoints( position, setField, time(), exclusionListAllPtr());
	
	if(!newInsertedPtr)
	{
		fatalErrorInFunction<<
		"  error in inserting points into pStruct. \n";
		return false;
	}

	auto& newInserted = newInsertedPtr();
	
	if(!shapeName_.insertSetElement(newInserted, shapes))
	{
		fatalErrorInFunction<<
		"  error in inserting shapes into sphereParticles system.\n";
		return false;
	}	

	if( !insertSphereParticles(shapes, newInserted) )
	{
		fatalErrorInFunction<< 
		"error in inserting shapes into the sphereParticles. \n";
		return false;
	}
	

	auto activeR = this->activeRange();

	Report(1)<< "Active range is "<<yellowText("["<<activeR.first<<", "<<activeR.second<<")")<<
	" and number of active points is "<< cyanText(this->numActive())<<
	" and pointStructure capacity is "<<cyanText(this->capacity())<<endReport;

	return true;
		
}