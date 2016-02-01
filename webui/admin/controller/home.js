var httpXS = angular.module('httpXS',['ngRoute','ui.bootstrap','ngAnimate']);

httpXS.config(function($routeProvider){
  $routeProvider
    .when('/', {
      templateUrl : 'tpls/home.html',
      controller  : 'homeCtrl'
    })
    .when('/create', {
      templateUrl : 'tpls/create_service.html',
      controller  : 'createCtrl'
    })
    .when('/viewService', {
      templateUrl : 'tpls/viewService.html',
      controller  : 'viewServiceCtrl'
    })
    .when('/createService', {
      templateUrl : 'tpls/createService.html',
      controller  : 'createServiceCtrl'
    });
});


httpXS.controller('homeCtrl',['$scope','$uibModal','$location',function($scope,$uibModal,$location){

  $scope.services = [
      {
        "name" : "Core-Service",
        "path" : "home/fabi",
        "running" : true
      },
      {
        "name" : "Service 2",
        "path" : "home/philip",
        "running" : false
      }
    ];

  $scope.add = function(){
    var uibModalInstance = $uibModal.open({
      templateUrl : 'tpls/create_service.html',
      controller  : 'createCtrl'
    });
  };

  $scope.viewService = function(service){
    $location.path('/viewService');
  }
}]);
