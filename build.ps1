# Install Dependencies
[CmdletBinding()]
param (
    [Parameter()]
    [Boolean] 
    $installDependencies=$true
)

Write-Host "[LOG] Building Game Engine and its dependencies..." -f Blue;
Write-Host "[LOG] Building Dependencie: Spline Collision Detection..." -f Blue;

cd splinecollisiondetection;
.\build.ps1 $False;
if ($?)
{
    Write-Host "[LOG] Build Spline Collision Detection Sucefully!" -f Green;
    cd ..;
    if(Test-Path "bin/SplineCollDet/Release/x64")
    {
        Copy-Item splinecollisiondetection/bin/Rel_LIB/x64/SplineCollisionDetection.lib bin/SplineCollDet/Release/x64;
    }
    else
    {
        makdir "bin/SplineCollDet/Release/x64";
        Copy-Item splinecollisiondetection/bin/Rel_LIB/x64/SplineCollisionDetection.lib bin/SplineCollDet/Release/x64;
    }
}
else {
    Write-Host "[ERROR] Failed to Build Spline Collision Detection" -f Red;
    cd ..;
    exit 1;
}
