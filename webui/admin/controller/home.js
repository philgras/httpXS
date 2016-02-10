var httpXS = angular.module('httpXS',['ngRoute','ui.bootstrap','ngAnimate','ngStorage']);

httpXS.config(function($routeProvider){
  $routeProvider
    .when('/', {
      templateUrl : 'tpls/home.html',
      controller  : 'homeCtrl'
    })
    .when('/service', {
      templateUrl : 'tpls/services.html',
      controller  : 'servicesCtrl'
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
  $scope.setCurrentLocation = function(activeA){
    switch (activeA) {
      case 'serviceOverview':
          $scope.serviceOverview = true;
        break;
      default:

    }
  }

}]);
