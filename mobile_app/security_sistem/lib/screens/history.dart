import 'package:flutter/material.dart';
import 'package:google_fonts/google_fonts.dart';
import 'package:not_a_weather_app/assets/app_colors.dart';
import 'package:provider/provider.dart';
import 'package:not_a_weather_app/screens/history_bloc.dart';

class History extends StatelessWidget {
  const History({Key? key}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    var width = MediaQuery.of(context).size.width;
    double height = MediaQuery.of(context).size.height;

    String getTime(HistoryBloc bloc, int index) {
      try {
        String time = DateTime.parse(bloc
            .listOfAttempts.entries
            .elementAt(index)
            .key).toString();
        return time;
      } on Exception catch (e) {
        return "Unknown";
      }
    }

    return ChangeNotifierProvider(
      create: (context) => HistoryBloc(),
      child: Consumer<HistoryBloc>(
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
                        color: Colors.black,
                        fontWeight: FontWeight.bold,
                        fontSize: 20.0,
                      ),
                    ),
                    ElevatedButton(
                      onPressed: () {
                        bloc.loadData();
                      },
                      style: ButtonStyle(
                        backgroundColor:
                            MaterialStateProperty.all(Colors.black),
                      ),
                      child: Text(
                        'Try again',
                        style: GoogleFonts.sourceSansPro(
                          color: AppColors.background,
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
                  bloc.loadData();
                },
                backgroundColor: Colors.black,
                child: const Icon(
                  Icons.refresh,
                  color: AppColors.background,
                ),
              ),
              body: Center(
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
                  'security sistem - history',
                  style: GoogleFonts.outfit(
                    color: Colors.white,
                    fontWeight: FontWeight.bold,
                    fontSize: 20.0,
                  ),
                ),
                centerTitle: true,
                backgroundColor: AppColors.background,
                elevation: 0,
              ),
              body: Padding(
                padding: const EdgeInsets.symmetric(horizontal: 32.0),
                child: Column(
                  children: [
                    SizedBox(
                      height: height * 0.8,
                      child: ListView.separated(
                        itemCount: bloc.listOfAttempts.length,
                        itemBuilder: (context, index) {
                          if (bloc.listOfAttempts.entries
                                  .elementAt(index)
                                  .key !=
                              null) {
                            return Container(
                              decoration: BoxDecoration(
                                color: Colors.white,
                                borderRadius: BorderRadius.circular(10.0),
                              ),
                              child: Padding(
                                padding: const EdgeInsets.all(8.0),
                                child: Column(
                                  crossAxisAlignment: CrossAxisAlignment.start,
                                  children: [
                                    Row(
                                      children: [
                                        Icon(
                                          Icons.online_prediction,
                                          color: bloc.listOfAttempts.entries
                                              .elementAt(index)
                                              .value[1],
                                          size: 36.0,
                                        ),
                                        Text(getTime(bloc, index))
                                      ],
                                    ),
                                    Text(
                                      bloc.listOfAttempts.entries
                                          .elementAt(index)
                                          .value[0]
                                          .split("at")[0],
                                      style: GoogleFonts.sourceSansPro(
                                        fontWeight: FontWeight.bold,
                                        fontSize: 16.0,
                                      ),
                                    ),
                                  ],
                                ),
                              ),
                            );
                          } else {
                            return const SizedBox();
                          }
                        },
                        separatorBuilder: (BuildContext context, int index) {
                          return const SizedBox(
                            height: 8.0,
                          );
                        },
                      ),
                    ),
                  ],
                ),
              ),
              floatingActionButton: FloatingActionButton(
                onPressed: () {
                  bloc.loadData();
                },
                backgroundColor: Colors.black,
                child: const Icon(
                  Icons.refresh,
                  color: AppColors.background,
                ),
              ),
            );
          }
        },
      ),
    );
  }
}
