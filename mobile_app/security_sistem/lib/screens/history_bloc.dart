import 'dart:math';
import 'package:flutter/material.dart';
import 'package:http/http.dart' as http;

import 'package:not_a_weather_app/assets/constants.dart';

class HistoryBloc extends ChangeNotifier {
  HistoryBloc() {
    notifyListeners();
    loadData();
  }

  bool isLoadingDone = false;
  bool isError = false;

  // List<String> history = [];
  Map<String, List<dynamic>> listOfAttempts = {};
  var response;
  String oldResponse = '';

  loadData() async {
    isError = false;
    isLoadingDone = false;
    notifyListeners();

    response = await http.get(Uri.parse(url));
    print(response.body);

    loadHistory();
  }

  loadHistory() {
    listOfAttempts = {};

    print("HERE${response.body.toString().split('STARTHISTORY')[1].substring(
          oldResponse.isEmpty
              ? 0
              : response.body.toString().lastIndexOf(oldResponse),
        ).split('ENDHISTORY')[0].split('<br>')}");

    prelucrareDate(
      response.body
          .toString()
          .split('STARTHISTORY')[1]
          .split('ENDHISTORY')[0]
          .split('<br>'),
    );

    isLoadingDone = true;
    notifyListeners();

    print('OLD RESPONSE$oldResponse');
  }

  prelucrareDate(List<String> history) {
      for (String item in history) {
        item = item.replaceAll(',', '');
        print(item);
        Color color;
        if (item.contains('turned on')) {
          color = Colors.green;
        }
        if (item.contains('turned off')) {
          color = Colors.blueGrey;
        } else if (item.contains('incorrect')) {
          color = Colors.red;
        } else if (item.contains('correct')) {
          color = const Color(0xFF00FF00);
        } else {
          color = const Color(0xFF000000);
        }
        if (item.length != 0) {
          String entryDate = '';
          if(item.split('at').length > 1)
            entryDate = item.split('at')[1].split(' ')[1].split('.')[0].replaceAll('T', ' ');
          listOfAttempts[entryDate == '' ? 'id: ${Random().nextInt(99999999)}' : entryDate] = [item, color];

        }
      }
  }
}
