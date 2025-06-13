// This is a basic Flutter widget test.
//
// To perform an interaction with a widget in your test, use the WidgetTester
// utility in the flutter_test package. For example, you can send tap and scroll
// gestures. You can also use WidgetTester to find child widgets in the widget
// tree, read text, and verify that the values of widget properties are correct.

import 'package:flutter_test/flutter_test.dart';

// STEP 1: Import your main app file
import 'package:flutter_app/main.dart';

void main() {
  testWidgets('Counter increments smoke test', (WidgetTester tester) async {
    // Build our app and trigger a frame.
    // STEP 2: Use the correct widget name here
    await tester.pumpWidget(const RobotControllerApp());

    // The default test looks for a '0' and a '1', which our app doesn't have.
    // This is fine, we can change this later if we write real tests.
    // For now, we'll just check that the AppBar title is there.
    expect(find.text('Robot UDP Controller'), findsOneWidget);
    expect(find.text('Speed: 200'), findsOneWidget);

    // You can delete the old test logic below if you want.
    // expect(find.text('0'), findsOneWidget);
    // expect(find.text('1'), findsNWidgets); // This was a typo in default, should be findsNothing

    // await tester.tap(find.byIcon(Icons.add));
    // await tester.pump();

    // expect(find.text('0'), findsNothing);
    // expect(find.text('1'), findsOneWidget);
  });
}
