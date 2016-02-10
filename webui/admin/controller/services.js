httpXS.controller('servicesCtrl',['$scope','$location','$uibModal','hostedServices','$localStorage',function($scope,$location,$uibModal,hostedServices,$localStorage){


  $scope.servicesDB = [
      {
        "name" : "Core-Service",
        "path" : "home/fabi",
        "running" : true,
        "conn" :  2
      },
      {
        "name" : "Service 2",
        "path" : "home/philip",
        "running" : false,
        "conn" : 1
      },
      {
        "name" : "Peak-Service",
        "path" : "home/philip",
        "running" : false,
        "conn" : 1
      },
      {
        "name" : "Service 23935793249723423497",
        "path" : "home/philip",
        "running" : false,
        "conn" : 1
      },
      {
        "name" : "Service 2",
        "path" : "home/philip",
        "running" : false,
        "conn" : 1
      }
    ];

    function smartTrim(str, length, delim, appendix) {
      if (str.length <= length) return str;

      var trimmedStr = str.substr(0, length+delim.length);

      var lastDelimIndex = trimmedStr.lastIndexOf(delim);
      if (lastDelimIndex >= 0) trimmedStr = trimmedStr.substr(0, lastDelimIndex);

      if (trimmedStr) trimmedStr += appendix;
      return trimmedStr;
  }
  var a = 'trimmedName';


  $scope.test = function(){for(var i = 0;i < $scope.servicesDB.length ; i++ ){
    $scope.servicesDB[i][a] = smartTrim($scope.servicesDB[i].name, 15 , '', '...');
    console.log("String was trimmed");
  }};

  $scope.test();
  $scope.services = $scope.servicesDB;



  $scope.add = function(){
    // var uibModalInstance = $uibModal.open({
    //   templateUrl : 'tpls/create_service.html',
    //   controller  : 'createCtrl'
    // });

  };

  $scope.viewService = function(i){
    hostedServices.setServiceObj($scope.services[i]);
    $localStorage.service = $scope.services[i];
    console.log($localStorage.service);
    $location.path('/viewService');
  }

}])
