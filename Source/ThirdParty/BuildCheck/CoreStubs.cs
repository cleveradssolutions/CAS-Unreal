// Stand-ins for the core types of Unreal Build Tool: EpicGames.Core in Unreal Engine 5,
// Tools.DotNETCommon in Unreal Engine 4.
//
// Json.Deserialize is a real reader here, so that the config of an Unreal Engine 4 build
// is parsed rather than faked: it maps the fields of the file onto the fields of the
// type, which is what the engine does, and says so when a field has no counterpart.
using System;
using System.Collections.Generic;
using System.IO;

#if UE_5_0_OR_LATER
namespace EpicGames.Core
#else
namespace Tools.DotNETCommon
#endif
{
    public class BuildException : Exception
    {
        public BuildException(string Message) : base(Message) { }
    }

    public enum LogEventType { Warning, Error }

    public static class Log
    {
        public static void TraceInformation(string Message) { Console.WriteLine(Message); }
        public static void WriteLineOnce(LogEventType Type, string Message) { Console.WriteLine(Message); }
        public static Microsoft.Extensions.Logging.ILogger Logger = new Microsoft.Extensions.Logging.ILogger();
    }

    public class FileReference
    {
        public string FullName;
        public FileReference(string Path) { FullName = Path; }
        public static FileReference Combine(DirectoryReference Dir, params string[] Parts)
        {
            var All = new List<string> { Dir.FullName };
            All.AddRange(Parts);
            return new FileReference(System.IO.Path.Combine(All.ToArray()));
        }
        public static bool Exists(FileReference File) { return System.IO.File.Exists(File.FullName); }
        public static void Copy(FileReference From, FileReference To, bool Overwrite) { }
        public static string ReadAllText(FileReference File) { return System.IO.File.ReadAllText(File.FullName); }
        public static void WriteAllText(FileReference File, string Text) { System.IO.File.WriteAllText(File.FullName, Text); }
        public static DateTime GetLastWriteTime(FileReference File) { return DateTime.Now; }
        public static void SetLastWriteTime(FileReference File, DateTime Time) { }
        public static FileReference FromString(string Path) { return new FileReference(Path); }
        public FileReference ChangeExtension(string Extension) { return this; }
        public string GetFileName() { return System.IO.Path.GetFileName(FullName); }
        public override string ToString() { return FullName; }
    }

    public class DirectoryReference
    {
        public string FullName;
        public DirectoryReference(string Path) { FullName = Path; }
        public static DirectoryReference Combine(DirectoryReference Dir, params string[] Parts)
        {
            var All = new List<string> { Dir.FullName };
            All.AddRange(Parts);
            return new DirectoryReference(System.IO.Path.Combine(All.ToArray()));
        }
        public static DirectoryReference FromFile(FileReference File) { return new DirectoryReference("."); }
        public override string ToString() { return FullName; }
    }

    public static class FileUtils
    {
        public static void ForceDeleteFile(FileReference File) { }
        public static void ForceDeleteDirectory(string Path) { }
        public static void ForceDeleteDirectory(DirectoryReference Dir) { }
        public static void CreateDirectoryTree(DirectoryReference Dir) { }
        public static void ForceMoveFile(FileReference From, FileReference To) { }
    }

    public class JsonObject
    {
        private System.Collections.Generic.Dictionary<string, object> Values = new System.Collections.Generic.Dictionary<string, object>();
        public static JsonObject Read(FileReference File) { return new JsonObject(); }
        public static JsonObject Parse(string Text) { return new JsonObject(); }
        public string GetStringField(string Name) { return ""; }
        public bool TryGetStringField(string Name, out string Value) { Value = null; return false; }
        public bool TryGetStringArrayField(string Name, out string[] Value) { Value = null; return false; }
        public bool TryGetObjectArrayField(string Name, out JsonObject[] Value) { Value = null; return false; }
        public JsonObject GetObjectField(string Name) { return new JsonObject(); }
        public JsonObject[] GetObjectArrayField(string Name) { return new JsonObject[0]; }
    }

    public static class Json
    {
        public static string Serialize(object Value, JsonSerializeOptions Options) { return "{}"; }

        public static T Deserialize<T>(string Text)
        {
            int Index = 0;
            object Parsed = TinyJson.Parse(Text, ref Index);
            return (T)Bind(typeof(T), Parsed);
        }

        static object Bind(Type Target, object Value)
        {
            if (Value == null) return null;
            if (Target == typeof(string)) return Convert.ToString(Value);
            if (Target == typeof(bool)) return Convert.ToBoolean(Value);
            if (Target == typeof(int)) return Convert.ToInt32(Value);

            if (Target.IsArray)
            {
                var Items = (List<object>)Value;
                var Result = Array.CreateInstance(Target.GetElementType(), Items.Count);
                for (int i = 0; i < Items.Count; i++)
                    Result.SetValue(Bind(Target.GetElementType(), Items[i]), i);
                return Result;
            }

            if (Target.IsGenericType && Target.GetGenericTypeDefinition() == typeof(List<>))
            {
                var Element = Target.GetGenericArguments()[0];
                var Result = (System.Collections.IList)Activator.CreateInstance(Target);
                foreach (var Item in (List<object>)Value)
                    Result.Add(Bind(Element, Item));
                return Result;
            }

            var Fields = (Dictionary<string, object>)Value;
            var Instance = Activator.CreateInstance(Target);
            foreach (var Pair in Fields)
            {
                var Field = Target.GetField(Pair.Key,
                    System.Reflection.BindingFlags.Public | System.Reflection.BindingFlags.Instance
                    | System.Reflection.BindingFlags.IgnoreCase);
                if (Field == null)
                {
                    Console.WriteLine("    [json] no field for '" + Pair.Key + "' in " + Target.Name);
                    continue;
                }
                Field.SetValue(Instance, Bind(Field.FieldType, Pair.Value));
            }
            return Instance;
        }
    }

    static class TinyJson
    {
        public static object Parse(string Text, ref int Index)
        {
            SkipSpace(Text, ref Index);
            char C = Text[Index];
            if (C == '{')
            {
                var Result = new Dictionary<string, object>();
                Index++;
                SkipSpace(Text, ref Index);
                if (Text[Index] == '}') { Index++; return Result; }
                while (true)
                {
                    SkipSpace(Text, ref Index);
                    string Key = (string)Parse(Text, ref Index);
                    SkipSpace(Text, ref Index);
                    Index++; // :
                    Result[Key] = Parse(Text, ref Index);
                    SkipSpace(Text, ref Index);
                    if (Text[Index] == ',') { Index++; continue; }
                    Index++; // }
                    return Result;
                }
            }
            if (C == '[')
            {
                var Result = new List<object>();
                Index++;
                SkipSpace(Text, ref Index);
                if (Text[Index] == ']') { Index++; return Result; }
                while (true)
                {
                    Result.Add(Parse(Text, ref Index));
                    SkipSpace(Text, ref Index);
                    if (Text[Index] == ',') { Index++; continue; }
                    Index++; // ]
                    return Result;
                }
            }
            if (C == '"')
            {
                Index++;
                var Builder = new System.Text.StringBuilder();
                while (Text[Index] != '"')
                {
                    if (Text[Index] == '\\') Index++;
                    Builder.Append(Text[Index]);
                    Index++;
                }
                Index++;
                return Builder.ToString();
            }
            if (Text.Substring(Index).StartsWith("true")) { Index += 4; return true; }
            if (Text.Substring(Index).StartsWith("false")) { Index += 5; return false; }
            if (Text.Substring(Index).StartsWith("null")) { Index += 4; return null; }
            int Start = Index;
            while (Index < Text.Length && "-+.eE0123456789".IndexOf(Text[Index]) >= 0) Index++;
            return double.Parse(Text.Substring(Start, Index - Start),
                System.Globalization.CultureInfo.InvariantCulture);
        }

        static void SkipSpace(string Text, ref int Index)
        {
            while (Index < Text.Length && char.IsWhiteSpace(Text[Index])) Index++;
        }
    }

    public enum JsonSerializeOptions { None }
}

namespace Microsoft.Extensions.Logging
{
    public class ILogger
    {
        public void LogInformation(string Message) { Console.WriteLine(Message); }
        public void LogWarning(string Message) { Console.WriteLine(Message); }
    }
}
