# Install Dependencies
[CmdletBinding()]
param (
    [Parameter()] [Boolean] $installExternalDependencies=$false,
    [Parameter()] [String] $Configuration="Release"
)

Write-Host "[LOG] Building Game Engine" -f Blue;

$hasLocalVcpkg = Test-Path "vcpkg";
if (-not ($hasLocalVcpkg))
{
    Write-Host "[LOG] Doesn't has local vcpkg." -f DarkYellow;
    Write-Host "[LOG] Searching for Global install of vcpkg..." -f Yellow;

    $hasGlobalVcpkg = Get-Command vcpkg;
    if ($hasGlobalVcpkg)
    {
        Write-Host "[LOG] Has Global install of vcpkg!" -f Green;
        Write-Host "[LOG] Installing eigen..." -f Yellow;
        vcpkg install eigen3:x64-windows;
        Write-Host "[LOG] Eigen installed!" -f Green;
        Write-Host "[LOG] Installing assimp..." -f Yellow;
        vcpkg install assimp:x64-windows;
        Write-Host "[LOG] Assimp installed!" -f Green;
    }
    else
    {
        Write-Host "[HINT] If you already has vcpkg install, please include in Windows Path Variables." -f Magenta;
        Write-Host "[LOG] Doesn't has vcpkg installed." -f DarkYellow;
        Write-Host "[LOG] Installing vcpkg..." -f Yellow;
        git clone https://github.com/Microsoft/vcpkg.git;
        .\vcpkg\bootstrap-vcpkg.bat;
        Write-Host "[LOG] Installing eigen..." -f Yellow;
        .\vcpkg\vcpkg install eigen3:x64-windows;
        Write-Host "[LOG] Eigen installed!" -f Green;
        Write-Host "[LOG] Installing assimp..." -f Yellow;
        .\vcpkg\vcpkg install assimp:x64-windows;
        Write-Host "[LOG] Assimp installed!" -f Green;
        .\vcpkg\vcpkg integrate install;
        if (-not ($?))
        {
            Write-Host "[ERROR] Could not install vcpkg or dependencies" -f Red;
            Write-Host "[HINT] This script has to be executed in Developer PowerShell for Visual Studio" -f Magenta;
            exit 1;
        }
    }
    Write-Host "[LOG] Dependencies installed!" -f Green;
}

if($installExternalDependencies)
{
    $solutionType = If ($Configuration.equals("Release")) { "Rel_LIB" } Else { "Deb_LIB" };

    Write-Host "[LOG] Building Dependencie: Spline Collision Detection..." -f Blue;

    if (Test-Path "splinecollisiondetection")
    {
        cd splinecollisiondetection;
    }
    else
    {
        Write-Host "[LOG] Has not downloaded submodule Spline Collision Detection" -f Yellow;
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

    .\build.ps1 $False $Configuration;
    if ($?)
    {
        Write-Host "[LOG] Build Spline Collision Detection Sucefully!" -f Green;
        cd ..;
        if(Test-Path "bin/SplineCollDet/$Configuration/x64")
        {
            Copy-Item splinecollisiondetection\bin\$solutionType\x64\SplineCollisionDetection.lib bin\SplineCollDet\$Configuration\x64\;
        }
        else
        {
            mkdir "bin/SplineCollDet/$Configuration/x64";
            Copy-Item splinecollisiondetection\bin\$solutionType\x64\SplineCollisionDetection.lib bin\SplineCollDet\$Configuration\x64\;
        }
        if($?)
        {
            Write-Host "[LOG] SplineCollisionDetection.lib was put on bin/SplineCollDet/$Configuration/x64" -f Green;
        }
        else
        {
            Write-Host "[ERROR] Failed copying SplineCollisionDetection.lib to bin/SplineCollDet/$Configuration/x64" -f Red;
        }
    }
    else {
        Write-Host "[ERROR] Failed to Build Spline Collision Detection" -f Red;
        cd ..;
        exit 1;
    }


    Write-Host "[LOG] Building Dependencie: GVDB Collision Detection..." -f Blue;

    if (Test-Path "GVDBergenCollisionDetection")
    {
        cd GVDBergenCollisionDetection;
    }
    else
    {
        Write-Host "[LOG] Has not downloaded submodule GVDB Collision Detection" -f Yellow;
        Write-Host "[LOG] Downloading submodule..." -f Blue;
        git submodule init;
        if($?)
        {
            git submodule update;
            cd GVDBergenCollisionDetection;
            Write-Host "[LOG] Download Sucess!" -f Green;
        }
        else
        {
            Write-Host "[ERROR] Failed initiating submodule..." -f Red;
            exit 1;
        }
    }

    .\build.ps1 $False $Configuration;
    if ($?)
    {
        Write-Host "[LOG] Build GVDB Collision Detection Sucefully!" -f Green;
        cd ..;
        if(Test-Path "bin/GVDBergenCollisionDetection/$Configuration/x64")
        {
            Copy-Item GVDBergenCollisionDetection\bin\$solutionType\x64\GVDBergenCollisionDetection.lib bin\GVDBergenCollisionDetection\$Configuration\x64\;
        }
        else
        {
            mkdir "bin/GVDBergenCollisionDetection/$Configuration/x64";
            Copy-Item GVDBergenCollisionDetection\bin\$solutionType\x64\GVDBergenCollisionDetection.lib bin\GVDBergenCollisionDetection\$Configuration\x64\;
        }
        if($?)
        {
            Write-Host "[LOG] GVDBergenCollisionDetection.lib was put on bin/GVDBergenCollisionDetection/$Configuration/x64" -f Green;
        }
        else
        {
            Write-Host "[ERROR] Failed copying GVDBergenCollisionDetection.lib to bin/GVDBergenCollisionDetection/$Configuration/x64" -f Red;
        }
    }
    else {
        Write-Host "[ERROR] Failed to Build GVDBergen Collision Detection" -f Red;
        cd ..;
        exit 1;
    }
}
