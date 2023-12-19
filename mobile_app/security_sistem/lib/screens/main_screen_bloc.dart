import 'package:flutter/material.dart';
import 'package:localstorage/localstorage.dart';
import 'package:flutter/cupertino.dart';
import 'package:http/http.dart' as http;
import 'package:not_a_weather_app/assets/constants.dart';

class MainBloc extends ChangeNotifier {
  MainBloc() {
    getAlarmState();
  }

  final storage = LocalStorage('history.json');
  bool isLoadingDone = false;
  bool isError = false;
  double lightIntensity = 0;
  Map<DateTime, List<dynamic>> listOfAttempts = {};
  var response;
  bool roomOneAlert = false;
  bool roomTwoAlert = false;
  bool isAlarmTurnedOn = false;
  String temperature = '0';

  setLight(double value) async {
    lightIntensity = value;
    if (value == 100) {
      http.get(Uri.parse('$url/H'));
    } else if (value == 0) {
      http.get(Uri.parse('$url/L'));
    }
    notifyListeners();
  }

  getAlarmState() async {
    isLoadingDone = false;
    response = await http.get(Uri.parse(url));
    response.body.toString().split('GETALARM:')[1].startsWith('0')
        ? isAlarmTurnedOn = false
        : isAlarmTurnedOn = true;
    print(response.body.toString().split('room1status:')[1]);
    roomOneAlert = response.body.toString().split('room1status:')[1].startsWith('0')
        ? false
        : true;
    roomTwoAlert = response.body.toString().split('room2status:')[1].startsWith('0')
        ? false
        : true;
    temperature = response.body.toString().split('temperature:')[1].split('ENDTEMPERATURE')[0];
    print(temperature);
    isLoadingDone = true;
    notifyListeners();
  }

  switchAlarm() async {
    isLoadingDone = false;
    notifyListeners();
    http.get(Uri.parse('$url/switch-alarm'));
    isAlarmTurnedOn = !isAlarmTurnedOn;
    isLoadingDone = true;
    // Future.delayed(Duration(seconds: 1));
    notifyListeners();
  }
}
