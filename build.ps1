# Install Dependencies
[CmdletBinding()]
param (
    [Parameter()] [Boolean] $installDependencies=$true
)

Write-Host "[LOG] Building Game Engine and its dependencies..." -f Blue;
Write-Host "[LOG] Building Dependencie: Spline Collision Detection..." -f Blue;

if (Test-Path "splinecollisiondetection")
{
    cd splinecollisiondetection;
}
else
{
    Write-Host "[ERROR] Has not downloaded submodule Spline Collision Detection" -f Red;
    Write-Host "[LOG] Downloading submodule..." -f Blue;
    git submodule init;
    if($?)
    {
        git submodule update;
        cd splinecollisiondetection;
        Write-Host "[LOG] Download Sucess!" -f Green;
    }
    else
    {
        Write-Host "[ERROR] Failed initiating submodule..." -f Red;
        exit 1;
    }
}

.\build.ps1 $False;
if ($?)
{
    Write-Host "[LOG] Build Spline Collision Detection Sucefully!" -f Green;
    cd ..;
    if(Test-Path "bin/SplineCollDet/Release/x64")
    {
        Copy-Item splinecollisiondetection\bin\Rel_LIB\x64\SplineCollisionDetection.lib bin\SplineCollDet\Release\x64\;
    }
    else
    {
        makdir "bin/SplineCollDet/Release/x64";
        Copy-Item splinecollisiondetection\bin\Rel_LIB\x64\SplineCollisionDetection.lib bin\SplineCollDet\Release\x64\;
    }
    if($?)
    {
        Write-Host "[LOG] SplineCollisionDetection.lib was put on bin/SplineCollDet/Release/x64" -f Green;
    }
    else
    {
        Write-Host "[ERROR] Failed copying SplineCollisionDetection.lib to bin/SplineCollDet/Release/x64" -f Red;
    }
}
else {
    Write-Host "[ERROR] Failed to Build Spline Collision Detection" -f Red;
    cd ..;
    exit 1;
}
