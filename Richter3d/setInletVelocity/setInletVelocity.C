/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | foam-extend: Open Source CFD
   \\    /   O peration     | Version:     4.0
    \\  /    A nd           | Web:         http://www.foam-extend.org
     \\/     M anipulation  | For copyright notice see file Copyright
-------------------------------------------------------------------------------
License
    This file is part of foam-extend.

    foam-extend is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation, either version 3 of the License, or (at your
    option) any later version.

    foam-extend is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with foam-extend.  If not, see <http://www.gnu.org/licenses/>.

Author
    Zeljko Tukovic, FSB Zagreb.  All rights reserved

\*----------------------------------------------------------------------------*/

#include "setInletVelocity.H"
#include "addToRunTimeSelectionTable.H"
#include "volFields.H"
#include "pointFields.H"
#include "boundBox.H"
#include "polyPatchID.H"
#include "fixedValueFvPatchFields.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(setInletVelocity, 0);

    addToRunTimeSelectionTable
    (
        functionObject,
        setInletVelocity,
        dictionary
    );
}


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

bool Foam::setInletVelocity::setVelocity()
{
    Info << "Seting inlet velocity" << endl;

    const fvMesh& mesh =
        time_.lookupObject<fvMesh>(regionName_);

    volVectorField& U =
        const_cast<volVectorField&>
        (
            mesh.lookupObject<volVectorField>("U")
        );


    word inletPatchName("inlet");

    polyPatchID inletPatch(inletPatchName, mesh.boundaryMesh());

    if (!inletPatch.active())
    {
        FatalErrorIn("restrictorBcAndReport::setBc()")
            << "Inlet patch name " << inletPatchName << " not found."
                << abort(FatalError);
    }

    label inletPatchIndex = inletPatch.index();

//     if
//     (
//         U.boundaryField()[inletPatchIndex].type()
//      == fixedValueFvPatchVectorField::typeName
//     )
    {
        fixedValueFvPatchVectorField& inletU =
            refCast<fixedValueFvPatchVectorField>
            (
                U.boundaryField()[inletPatchIndex]
            );

        const vectorField& Cf = mesh.Cf().boundaryField()[inletPatchIndex];

        scalar Umax = 10; //0.3;

        scalar t = time_.value() + time_.deltaT().value();

        scalar T = 1; //0.2;

        if (t < T)
        {
            Umax = Umax*(1.0 - cos(M_PI*t/T))/2.0;
        }

        Info << "Umax = " << Umax << endl;

        forAll(Cf, faceI)
        {
            scalar y = Cf[faceI].y();
            scalar z = Cf[faceI].z();
            scalar Z = z + 0.405;

            inletU[faceI] =
                Umax*y*(0.4-y)*(sqr(0.405)-sqr(Z))*vector(1,0,0)
               /(sqr(0.2)*sqr(0.405));
        }
    }

    return true;
}

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::setInletVelocity::setInletVelocity
(
    const word& name,
    const Time& t,
    const dictionary& dict
)
:
    functionObject(name),
    name_(name),
    time_(t),
    regionName_(polyMesh::defaultRegion)
{
    if (dict.found("region"))
    {
        dict.lookup("region") >> regionName_;
    }
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool Foam::setInletVelocity::start()
{
    return setVelocity();
}


bool Foam::setInletVelocity::execute()
{
    return setVelocity();
}


bool Foam::setInletVelocity::read(const dictionary& dict)
{
    if (dict.found("region"))
    {
        dict.lookup("region") >> regionName_;
    }

    return true;
}

// ************************************************************************* //
