/* 
 * Copyright (c) 2013, 2017 Max Planck Institute for Biological Cybernetics
 * All rights reserved.
 * 
 * This file is part of MUGS - Mobile and Unrestrained Gazetracking Software.
 *
 * MUGS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MUGS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MUGS.  If not, see <http://www.gnu.org/licenses/>. 
 */

#ifndef LINEAR_EYE_MODEL_H
#define LINEAR_EYE_MODEL_H

#include <vector>
#include <Eigen/Dense>

#include <mug/eye_model.h>
#include "mug/sample.h"

namespace mug
{
    /** 
     * \brief   Eye model using linear regression to create linear mapping from  
     *          pupil positions to gaze angles. 
     * \note    This model is inaccurate for large eye movements but 
     *          computationally very efficient. 
     * \author  Bjoern Browatzki and Jonas Ditz
     */
    class EyeModelLinear : public EyeModel
    {
    public:
        /**
	 * \brief Constructor of the EyeModelLinear class.
	 */
	EyeModelLinear()
	{
	    this->mt = EYE_LEFT;
	}
	
	/**
	 * \brief Constructor of the EyeModelLinear class.
	 * \param[in] mt ModelType that will be used for this instance of EyeModelLinear.
	 */
	EyeModelLinear(ModelType mt)
	{
	    this->mt = mt;
	}
	
	/**
	 * \brief Setter function for the member variable mt
	 * \param[in] mt ModelType value for member variable mt
	 */
	void setModelType (ModelType mt)
	{
	    this->mt = mt;
	}
	
	/**
	 * \brief Getter function for the member variable mt
	 */
	ModelType getModelType () const { return this->mt; }

        /**
         * \brief Getter function for the approach that this eye model is implemented for.
         */
        Approach getApproach () const { return GEOMETRIC; }
	
	/** 
         * \brief Perform linear regression to fit pupil positions to gaze angles (yaw, pitch).
	 *        This is just a functionless auxiliary function.
         * \param[in] samples Vector of mug::Sample containing pupil image positions
         */
        void fit(const std::vector<Sample> &samples);
	
        /** 
         * \brief Perform linear regression to fit pupil positions to gaze angles (yaw, pitch).
         * \param[in] pupilPositions Vector of 2D positions in the eye tracker camera image 
         * \param[in] gazeAngles Vector of (yaw, pitch) tuples
         */
        void fit(const std::vector<Eigen::Vector2f> &pupilPositions,  
                 const std::vector<Eigen::Vector2f> &gazeAngles);
	
	/** 
         * \brief Predict gaze angles based on pupil position
         * \param[in] s Sample object containing UV pupil position 
         * \return 2D vector containing yaw and pitch angle in radians.
         */
	Eigen::Vector2f predict(const Sample &s) const
	{
	    switch(this->mt)
	    {
		case EYE_RIGHT:
		{
		    Eigen::VectorXf pupil = Eigen::VectorXf(2);
	            pupil[0] = s.px_right;
	            pupil[1] = s.py_right;
	            return predict(pupil);
		}
		default:
		{
	            Eigen::VectorXf pupil = Eigen::VectorXf(2);
	            pupil[0] = s.px_left;
	            pupil[1] = s.py_left;
	            return predict(pupil);
		}
	    }
	}
	
	/**
	 * \brief Calculate confidence in predicted gaze angles. Confidence
	 *        calculation for geometric models is not implemented, currently.
	 * \param[in] s Sample object containing UV pupil position.
	 * \return Confidence in predicted ganze angles.
	 */
	inline double getConfidence(const Sample &s) const
	{
	    return 0.0;
	};

    private:
        Eigen::Vector2f modelYaw;       /// coefficients of linear model for yaw 
        Eigen::Vector2f modelPitch;     /// coefficients of linear model for pitch
        ModelType mt;                   /// specifies which eye should be used for regression
       
        /** 
         * \brief Computes simple linear regression.
         * \param[in] X Row matrix of input data.
         * \param[in] Y Row matrix of target data.
         * \return Parameters of fitted line
         */
        Eigen::Vector2f simpleRegression(const Eigen::VectorXf &X, const Eigen::VectorXf &Y);
	
	/** 
         * \brief Predict gaze angles based on pupil position based on linear model.
         * \param[in] pupil UV pupil position. Pupil is expected to be 2D vector (monocular).
         * \return 3D vector containing yaw and pitch angle in radians.
         */
        inline Eigen::Vector2f predict(const Eigen::VectorXf &pupil) const 
        {
            return Eigen::Vector2f(
                    modelYaw[0]   * pupil[0] + modelYaw[1],
                    modelPitch[0] * pupil[1] + modelPitch[1]);
        };
    };
}

#endif

