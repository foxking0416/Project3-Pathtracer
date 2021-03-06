// CIS565 CUDA Raytracer: A parallel raytracer for Patrick Cozzi's CIS565: GPU Computing at the University of Pennsylvania
// Written by Yining Karl Li, Copyright (c) 2012 University of Pennsylvania
// This file includes code from:
// Yining Karl Li's TAKUA Render, a massively parallel pathtracing renderer: http://www.yiningkarlli.com

#ifndef INTERACTIONS_H
#define INTERACTIONS_H

#include "intersections.h"

struct Fresnel {
  float reflectionCoefficient;
  float transmissionCoefficient;
};

struct AbsorptionAndScatteringProperties{
    glm::vec3 absorptionCoefficient;
    float reducedScatteringCoefficient;
};

// Forward declaration
__host__ __device__ bool calculateScatterAndAbsorption(ray& r, float& depth, AbsorptionAndScatteringProperties& currentAbsorptionAndScattering, glm::vec3& unabsorbedColor, material m, float randomFloatForScatteringDistance, float randomFloat2, float randomFloat3);
__host__ __device__ glm::vec3 getRandomDirectionInSphere(float xi1, float xi2);
__host__ __device__ glm::vec3 calculateTransmission(glm::vec3 absorptionCoefficient, float distance);
__host__ __device__ glm::vec3 calculateTransmissionDirection(glm::vec3 normal, glm::vec3 incident, float incidentIOR, float transmittedIOR);
__host__ __device__ glm::vec3 calculateReflectionDirection(glm::vec3 normal, glm::vec3 incident);
__host__ __device__ Fresnel calculateFresnel(glm::vec3 normal, glm::vec3 incident, float incidentIOR, float transmittedIOR, glm::vec3 reflectionDirection, glm::vec3 transmissionDirection);
__host__ __device__ int calculateSelfBSDF(ray& r, staticGeom geom, glm::vec3 intersectIn, glm::vec3 intersectOut, glm::vec3 normal, material m, float xi1, float xi2, int& restDepth);
__host__ __device__ glm::vec3 calculateRandomDirectionInHemisphere(glm::vec3 normal, float xi1, float xi2);

// TODO (OPTIONAL): IMPLEMENT THIS FUNCTION
__host__ __device__ glm::vec3 calculateTransmission(glm::vec3 absorptionCoefficient, float distance) {
  return glm::vec3(0,0,0);
}

// TODO (OPTIONAL): IMPLEMENT THIS FUNCTION
__host__ __device__ bool calculateScatterAndAbsorption(ray& r, float& depth, AbsorptionAndScatteringProperties& currentAbsorptionAndScattering,
                                                        glm::vec3& unabsorbedColor, material m, float randomFloatForScatteringDistance, float randomFloat2, float randomFloat3){
  return false;
}

// TODO (OPTIONAL): IMPLEMENT THIS FUNCTION
__host__ __device__ glm::vec3 calculateTransmissionDirection(glm::vec3 normal, glm::vec3 incident, float incidentIOR, float transmittedIOR) {
  return glm::vec3(0,0,0);
}

// TODO (OPTIONAL): IMPLEMENT THIS FUNCTION
__host__ __device__ glm::vec3 calculateReflectionDirection(glm::vec3 normal, glm::vec3 incident) {
  //nothing fancy here

  return glm::normalize(incident - 2.0f * normal * glm::dot(normal, incident));
}

// TODO (OPTIONAL): IMPLEMENT THIS FUNCTION
__host__ __device__ Fresnel calculateFresnel(glm::vec3 normal, glm::vec3 incident, float incidentIOR, float transmittedIOR, glm::vec3 reflectionDirection, glm::vec3 transmissionDirection) {
  Fresnel fresnel;

  fresnel.reflectionCoefficient = 1;
  fresnel.transmissionCoefficient = 0;
  return fresnel;
}

// LOOK: This function demonstrates cosine weighted random direction generation in a sphere!
__host__ __device__ glm::vec3 calculateRandomDirectionInHemisphere(glm::vec3 normal, float xi1, float xi2) {
    
    // Crucial difference between this and calculateRandomDirectionInSphere: THIS IS COSINE WEIGHTED!
    
    float up = sqrt(xi1); // cos(theta)
    float over = sqrt(1 - up * up); // sin(theta)
    float around = xi2 * TWO_PI;//phi
    
    // Find a direction that is not the normal based off of whether or not the normal's components are all equal to sqrt(1/3) or whether or not at least one component is less than sqrt(1/3). Learned this trick from Peter Kutz.
    
    glm::vec3 directionNotNormal;
    if (abs(normal.x) < SQRT_OF_ONE_THIRD) {
	//if (abs(normal.x) <= abs(normal.y) && abs(normal.x) <= abs(normal.z)){
      directionNotNormal = glm::vec3(1, 0, 0);
    } else if (abs(normal.y) < SQRT_OF_ONE_THIRD) {
	//}else if (abs(normal.y)<=abs(normal.x) && abs(normal.y)<=abs(normal.z)){
      directionNotNormal = glm::vec3(0, 1, 0);
    } else {
      directionNotNormal = glm::vec3(0, 0, 1);
    }
    
    // Use not-normal direction to generate two perpendicular directions
    glm::vec3 perpendicularDirection1 = glm::normalize(glm::cross(normal, directionNotNormal));
    glm::vec3 perpendicularDirection2 = glm::normalize(glm::cross(normal, perpendicularDirection1));
    
    return ( up * normal ) + ( cos(around) * over * perpendicularDirection1 ) + ( sin(around) * over * perpendicularDirection2 );
    
}

// TODO: IMPLEMENT THIS FUNCTION
// Now that you know how cosine weighted direction generation works, try implementing 
// non-cosine (uniform) weighted random direction generation.
// This should be much easier than if you had to implement calculateRandomDirectionInHemisphere.
//xi1 -1 ~ 1   xi2: 0~ 1 
__host__ __device__ glm::vec3 getRandomDirectionInSphere(float xi1, float xi2) {
	float r = sqrt(1.0f - xi1 * xi1);//sin(theta)	
	float phi = xi2 * TWO_PI;
	glm::vec3 v(cos(phi) * r, sin(phi) * r, xi1);


  return v;
}

// TODO (PARTIALLY OPTIONAL): IMPLEMENT THIS FUNCTION
// Returns 0 if diffuse scatter, 1 if reflected, 2 if transmitted.
__host__ __device__ int calculateBSDF(ray& r, glm::vec3 intersect, glm::vec3 normal, glm::vec3 emittedColor,
                                       AbsorptionAndScatteringProperties& currentAbsorptionAndScattering,
                                       glm::vec3& color, glm::vec3& unabsorbedColor, material m){


 
  return 1;
};

__host__ __device__ int calculateSelfBSDF(ray& r, staticGeom geom, glm::vec3 intersectIn, glm::vec3 intersectOut, glm::vec3 normal, material m, float xi1, float xi2, int& restDepth){

	int type;
	if(m.hasScatter == 1){
		type = 3;
	}
	else{
		if(m.hasReflective == 1 && m.hasRefractive == 1){
			if(xi1 < 0.5)
				type = 1;
			else
				type = 2;
		}else if(m.hasReflective == 1 && m.hasRefractive == 0){
			type = 1;
		}else if(m.hasReflective == 0 && m.hasRefractive == 1){
			type = 2;
		}else{
			type = 3;
		}
	}

	if(type == 1){
		r.origin = intersectOut;
		r.direction = calculateReflectionDirection(normal, r.direction);

		return 1;
	}
 
	else if(type == 2){

		//from air to object
		float airToObjCosTheta1 = -1 * glm::dot(r.direction, normal);
		float airToObjCosTheta2Square = 1 - (1 / pow(m.indexOfRefraction, 2)) * (1 - pow(airToObjCosTheta1, 2));

		ray insideRay;
		insideRay.direction = glm::normalize(r.direction / m.indexOfRefraction + (airToObjCosTheta1 / m.indexOfRefraction - sqrt(airToObjCosTheta2Square)) * normal);
		insideRay.origin = intersectIn;

		while(restDepth > 0){
			float dis = -1;
			glm::vec3 objIntersectPt(0, 0, 0);
			glm::vec3 objIntersectN(0, 0, 0);
			switch(geom.type){
				case SPHERE:
					dis = sphereIntersectionTest(geom, insideRay, objIntersectPt, objIntersectN);
					break;
				case CUBE:
					dis = boxIntersectionTest(geom, insideRay, objIntersectPt, objIntersectN);
					break;
				case MESH:
					break;
			}

			
			//from object back to air
			float refractionRatioObjToAir = 1 / m.indexOfRefraction;
			float objToAirCosTheta1 = -1 * glm::dot(insideRay.direction, objIntersectN);
			float objToAirCosTheta2Square = 1 - (1 / pow(refractionRatioObjToAir, 2)) * (1 - pow(objToAirCosTheta1, 2));

			if(objToAirCosTheta2Square > 0){//��g��z
				r.origin = objIntersectPt +  insideRay.direction * (float)RAY_BIAS_AMOUNT;
				r.direction = glm::normalize(insideRay.direction / refractionRatioObjToAir + (objToAirCosTheta1 / refractionRatioObjToAir - sqrt(objToAirCosTheta2Square)) * objIntersectN);
				break;
			}
			else if(objToAirCosTheta2Square == 0){//�{�ɨ�
				r.origin = intersectOut;
				r.direction = glm::vec3(0, 0, 0);
				restDepth = 0;
				break;
			}
			else{
				insideRay.origin = objIntersectPt - insideRay.direction * (float)RAY_BIAS_AMOUNT;
				insideRay.direction = insideRay.direction - 2.0f * objIntersectN * glm::dot(objIntersectN, insideRay.direction);
				r.origin = intersectOut;
				r.direction = glm::vec3(0, 0, 0);
				//restDepth = 0;
				restDepth--;
			}
		}
		return 2;
	}

	else if(type == 3){
		if(m.reducedScatterCoefficient > 1){
			if(xi1 > 1 / m.reducedScatterCoefficient){
				restDepth = 0;
				return 0;
			}
		}


		 //1 /m.reducedScatterCoefficient1
		r.origin = intersectOut;
		r.direction = glm::normalize(calculateRandomDirectionInHemisphere(normal, xi1, xi2));

		return 0;
	}

  return 1;
};

#endif
