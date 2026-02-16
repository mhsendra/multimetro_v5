import os

ruta ="C:\\ArduinoCLI\\firmware_multimetro_v5"

def listar_directorio(ruta):
    """
    Lista todos los archivos y carpetas en la ruta dada.
    """
    try:
        elementos = os.listdir(ruta)
        if not elementos:
            print(f"El directorio '{ruta}' está vacío.")
            return
        
        print(f"Contenido de '{ruta}':")
        for elem in elementos:
            ruta_completa = os.path.join(ruta, elem)
            if os.path.isdir(ruta_completa):
                print(f"[DIR]  {elem}")
            else:
                print(f"[FILE] {elem}")
    except FileNotFoundError:
        print(f"Error: El directorio '{ruta}' no existe.")
    except PermissionError:
        print(f"Error: Permiso denegado para acceder a '{ruta}'.")

if __name__ == "__main__":
    ruta = input("Introduce la ruta del directorio a listar: ").strip()
    listar_directorio(ruta)
