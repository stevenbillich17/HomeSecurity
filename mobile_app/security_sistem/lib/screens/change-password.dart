import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:http/http.dart' as http;
import 'package:not_a_weather_app/assets/app_colors.dart';
import 'package:not_a_weather_app/assets/constants.dart';

class ChangePassword extends StatelessWidget {
  const ChangePassword({Key? key}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    TextEditingController controller = TextEditingController();

    return Scaffold(
      backgroundColor: AppColors.background,
      appBar: AppBar(
        title: const Text('Change Password'),
        backgroundColor: AppColors.background,
      ),
      body: Padding(
        padding: const EdgeInsets.symmetric(horizontal: 26.0),
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            TextField(
              decoration: const InputDecoration(
                border: OutlineInputBorder(
                  borderSide: BorderSide(color: Colors.white),
                ),
                enabledBorder: OutlineInputBorder(
                  borderSide: BorderSide(color: Colors.white),
                ),
                counterStyle: TextStyle(color: Colors.white),
              ),
              style: const TextStyle(color: Colors.white),
              strutStyle: const StrutStyle(
                height: 1.5,
              ),
              maxLength: 4,
              keyboardType: TextInputType.number,
              inputFormatters: <TextInputFormatter>[
                FilteringTextInputFormatter.digitsOnly
              ],
              controller: controller,
              cursorColor: Colors.white,
            ),
            MaterialButton(
              onPressed: () {
                if (controller.text.length == 4) {
                  http.get(
                    Uri.parse('$url/change-password-${controller.text}'),
                  );
                  Future.delayed(const Duration(seconds: 2));
                  Navigator.pop(context);
                } else {
                  ScaffoldMessenger.of(context).showSnackBar(
                    const SnackBar(
                      content: Text('Password must be 4 digits long'),
                    ),
                  );
                }
              },
              child: const Text('Submit', style: TextStyle(color: Colors.white)),
            ),
          ],
        ),
      ),
    );
  }
}
