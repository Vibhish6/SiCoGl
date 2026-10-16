param([ValidateSet('Debug','Release')][string]$Configuration = 'Release')
$ErrorActionPreference = 'Stop'
cmake -S $PSScriptRoot/.. -B $PSScriptRoot/../build
cmake --build $PSScriptRoot/../build --config $Configuration --parallel
