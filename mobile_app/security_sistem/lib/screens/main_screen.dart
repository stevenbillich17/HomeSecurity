import 'package:flutter/material.dart';
import 'package:google_fonts/google_fonts.dart';
import 'package:not_a_weather_app/assets/app_colors.dart';
import 'package:not_a_weather_app/screens/change-password.dart';
import 'package:provider/provider.dart';
import 'package:not_a_weather_app/screens/history.dart';
import 'package:not_a_weather_app/screens/main_screen_bloc.dart';

class Main extends StatelessWidget {
  const Main({Key? key}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return ChangeNotifierProvider(
      create: (context) => MainBloc(),
      child: Consumer<MainBloc>(
        builder: (context, bloc, child) {
          if (bloc.isError) {
            return Scaffold(
              backgroundColor: AppColors.background,
              body: Padding(
                padding: const EdgeInsets.symmetric(horizontal: 40.0),
                child: Column(
                  mainAxisAlignment: MainAxisAlignment.center,
                  children: [
                    Text(
                      'There was an error while loading the data.',
                      textAlign: TextAlign.center,
                      style: GoogleFonts.sourceSansPro(
                        color: Colors.white,
                        fontWeight: FontWeight.bold,
                        fontSize: 20.0,
                      ),
                    ),
                    ElevatedButton(
                      onPressed: () {
                        // bloc.loadData();
                      },
                      style: ButtonStyle(
                        backgroundColor:
                        MaterialStateProperty.all(Colors.black),
                      ),
                      child: Text(
                        'Try again',
                        style: GoogleFonts.sourceSansPro(
                          color: Colors.white,
                          fontWeight: FontWeight.bold,
                          fontSize: 20.0,
                        ),
                      ),
                    ),
                  ],
                ),
              ),
            );
          } else if (!bloc.isLoadingDone) {
            return Scaffold(
              backgroundColor: AppColors.background,
              floatingActionButton: FloatingActionButton(
                onPressed: () {
                  bloc.getAlarmState();
                },
              ),
              body: const Center(
                child: Padding(
                  padding: EdgeInsets.all(32.0),
                  child: CircularProgressIndicator(
                    color: Colors.black,
                  ),
                ),
              ),
            );
          } else {
            return Scaffold(
              backgroundColor: AppColors.background,
              appBar: AppBar(
                title: Text(
                  'security sistem',
                  style: GoogleFonts.outfit(
                    color: Colors.white,
                    fontWeight: FontWeight.bold,
                    fontSize: 20.0,
                  ),
                ),
                centerTitle: true,
                backgroundColor: AppColors.background,
                elevation: 0,
                leading: IconButton(
                  onPressed: () {
                    Navigator.push(
                      context,
                      MaterialPageRoute(builder: (context) => const History()),
                    );
                  },
                  icon: const Icon(
                    Icons.history,
                    color: Colors.white,
                    size: 32.0,
                  ),
                ),
              ),
              body: Padding(
                padding: const EdgeInsets.symmetric(horizontal: 32.0),
                child: SingleChildScrollView(
                  child: Column(
                    children: [
                      AlarmWidget(
                        bloc: bloc,
                      ),
                      RoomOne(
                        bloc: bloc,
                      ),
                      RoomTwo(bloc: bloc),
                      const SizedBox(
                        height: 16.0,
                      ),
                      Text(
                        'Settings',
                        style: GoogleFonts.outfit(
                          fontSize: 24.0,
                          color: Colors.white,
                        ),
                      ),
                      ElevatedButton(
                        onPressed: () =>
                            Navigator.push(
                              context,
                              MaterialPageRoute(
                                builder: (context) => const ChangePassword(),
                              ),
                            ),
                        style: ButtonStyle(
                          backgroundColor:
                          MaterialStateProperty.all(const Color(0xFF47495e)),
                          shape: MaterialStateProperty.all(
                            RoundedRectangleBorder(
                              borderRadius: BorderRadius.circular(20.0),
                            ),
                          ),
                        ),
                        child: const Text('Change password'),
                      ),
                    ],
                  ),
                ),
              ),
              floatingActionButton: FloatingActionButton(
                onPressed: () {
                  bloc.getAlarmState();
                },
                backgroundColor: Colors.black,
                child: const Icon(
                  Icons.refresh,
                  color: AppColors.background,
                ),
              ),
            );
          }
        }
      )
    );
  }
}

class RoomOne extends StatelessWidget {
  const RoomOne({
    super.key,
    required this.bloc,
  });

  final MainBloc bloc;

  @override
  Widget build(BuildContext context) {
    return Column(
      children: [
        const SizedBox(
          height: 16.0,
        ),
        Text(
          'Room 1',
          style: GoogleFonts.outfit(fontSize: 24.0, color: Colors.white),
        ),
        const SizedBox(
          height: 8.0,
        ),
        Container(
          height: 70,
          width: double.infinity,
          decoration: BoxDecoration(
            color: const Color(0xFF47495e),
            borderRadius: BorderRadius.circular(20.0),
          ),
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              Padding(
                padding: const EdgeInsets.symmetric(
                  vertical: 8.0,
                  horizontal: 16.0,
                ),
                child: Row(
                  mainAxisAlignment: MainAxisAlignment.spaceBetween,
                  children: [
                    Row(
                      mainAxisAlignment: MainAxisAlignment.start,
                      children: const [
                        Icon(
                          Icons.security,
                          color: Colors.white,
                        ),
                        SizedBox(
                          width: 8.0,
                        ),
                        Text('Alarm status', style: TextStyle(color: Colors.white)),
                      ],
                    ),
                    Text(
                      bloc.roomOneAlert ? 'ON' : 'OFF',
                      style: GoogleFonts.sourceSansPro(
                        color: bloc.roomOneAlert ? Colors.red : Colors.green,
                        fontWeight: FontWeight.bold,
                        fontSize: 20.0,
                      ),
                    )
                  ],
                ),
              ),
            ],
          ),
        ),
      ],
    );
  }
}

class RoomTwo extends StatelessWidget {
  const RoomTwo({
    super.key,
    required this.bloc,
  });

  final MainBloc bloc;

  @override
  Widget build(BuildContext context) {
    return Column(
      children: [
        const SizedBox(
          height: 16.0,
        ),
        Text(
          'Room 2',
          style: GoogleFonts.outfit(fontSize: 24.0, color: Colors.white),
        ),
        const SizedBox(
          height: 8.0,
        ),
        Container(
          height: 70,
          width: double.infinity,
          decoration: BoxDecoration(
            color: const Color(0xFF47495e),
            borderRadius: BorderRadius.circular(20.0),
          ),
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              Padding(
                padding: const EdgeInsets.symmetric(
                  vertical: 8.0,
                  horizontal: 16.0,
                ),
                child: Row(
                  mainAxisAlignment: MainAxisAlignment.spaceBetween,
                  children: [
                    Row(
                      mainAxisAlignment: MainAxisAlignment.start,
                      children: const [
                        Icon(
                          Icons.security,
                          color: Colors.white,
                        ),
                        SizedBox(
                          width: 8.0,
                        ),
                        Text('Alarm status', style: TextStyle(color: Colors.white)),
                      ],
                    ),
                    Text(
                      bloc.roomTwoAlert ? 'ON' : 'OFF',
                      style: GoogleFonts.sourceSansPro(
                        color: bloc.roomTwoAlert ? Colors.red : Colors.green,
                        fontWeight: FontWeight.bold,
                        fontSize: 20.0,
                      ),
                    )
                  ],
                ),
              ),
            ],
          ),
        ),
        SizedBox(
          height: 16.0,
        ),
        Container(
          height: 70,
          width: double.infinity,
          decoration: BoxDecoration(
            color: const Color(0xFF47495e),
            borderRadius: BorderRadius.circular(20.0),
          ),
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              Padding(
                padding: const EdgeInsets.symmetric(
                  vertical: 8.0,
                  horizontal: 16.0,
                ),
                child: Row(
                  mainAxisAlignment: MainAxisAlignment.spaceBetween,
                  children: [
                    Row(
                      mainAxisAlignment: MainAxisAlignment.start,
                      children: const [
                        Icon(
                          Icons.thermostat,
                          color: Colors.white,
                        ),
                        SizedBox(
                          width: 8.0,
                        ),
                        Text('Temperature', style: TextStyle(color: Colors.white)),
                      ],
                    ),
                    Text(
                      '${bloc.temperature}°C',
                      style: GoogleFonts.sourceSansPro(
                        color: Colors.white,
                        fontWeight: FontWeight.bold,
                        fontSize: 20.0,
                      ),
                    )
                  ],
                ),
              ),
            ],
          ),
        ),
      ],
    );
  }
}

class AlarmWidget extends StatelessWidget {
  const AlarmWidget({super.key, required this.bloc});

  final MainBloc bloc;

  @override
  Widget build(BuildContext context) {
    return Column(
      children: [
        Text(
          'Alarm',
          style: GoogleFonts.outfit(fontSize: 24.0, color: Colors.white),
        ),
        const SizedBox(
          height: 16.0,
        ),
        Container(
          height: 70,
          width: double.infinity,
          decoration: BoxDecoration(
            color: const Color(0xFF47495e),
            borderRadius: BorderRadius.circular(20.0),
          ),
          child: Column(
            children: [
              Padding(
                padding: const EdgeInsets.symmetric(
                  vertical: 8.0,
                  horizontal: 16.0,
                ),
                child: Row(
                  mainAxisAlignment: MainAxisAlignment.spaceBetween,
                  children: [
                    Row(
                      mainAxisAlignment: MainAxisAlignment.start,
                      children: [
                        Icon(
                          Icons.light_mode,
                          color: Colors.orangeAccent.shade400,
                        ),
                        const SizedBox(
                          width: 8.0,
                        ),
                        const Text('Light', style: TextStyle(color: Colors.white,),),
                      ],
                    ),
                    Switch(
                      value: bloc.lightIntensity == 100 ? true : false,
                      onChanged: (bool newValue) {
                        bloc.setLight(newValue ? 100 : 0);
                      },
                    )
                  ],
                ),
              ),
            ],
          ),
        ),
        const SizedBox(
          height: 8.0,
        ),
        Container(
          height: 70,
          width: double.infinity,
          decoration: BoxDecoration(
            color: const Color(0xFF47495e),
            borderRadius: BorderRadius.circular(20.0),
          ),
          child: Column(
            children: [
              Padding(
                padding: const EdgeInsets.symmetric(
                  vertical: 8.0,
                  horizontal: 16.0,
                ),
                child: Row(
                  mainAxisAlignment: MainAxisAlignment.spaceBetween,
                  children: [
                    Row(
                      mainAxisAlignment: MainAxisAlignment.start,
                      children: const [
                        Icon(
                          Icons.security,
                          color: Colors.white,
                        ),
                        SizedBox(
                          width: 8.0,
                        ),
                        Text('Alarm', style: TextStyle(color: Colors.white,),),
                      ],
                    ),
                    Switch(
                      value: bloc.isAlarmTurnedOn,
                      onChanged: (bool newValue) {
                        bloc.switchAlarm();
                      },
                    )
                  ],
                ),
              ),
            ],
          ),
        ),
      ],
    );
  }
}
