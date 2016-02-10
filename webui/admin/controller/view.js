httpXS.controller('viewServiceCtrl',['$scope','$location','hostedServices','$localStorage',function($scope,$location,hostedServices,$localStorage){

  if (hostedServices.getServiceObj().length == undefined ) {
    $scope.services = $localStorage.service;
  }
  else{
    $scope.services = hostedServices.getServiceObj();
  }

  $scope.return = function(path){
    $location.path(path);
  }
}]);
