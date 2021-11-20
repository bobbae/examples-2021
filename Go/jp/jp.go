package jp

import (
	"encoding/json"
	"errors"
	"fmt"
	"reflect"
	"regexp"
	"strconv"
	"strings"
)

var re = regexp.MustCompile(`^([^\s\[]+)?(\[[0-9]+\])?$`)

func JSONPathos(v interface{}, jp string) (reflect.Kind, string, error) {
	if jp == "" {
		return reflect.Invalid, "", errors.New("invalid path")
	}
	for _, token := range strings.Split(jp, "/") {
		sl := re.FindAllStringSubmatch(token, -1)
		if len(sl) == 0 {
			return reflect.Invalid, "", errors.New("invalid path")
		}
		ss := sl[0]
		if ss[1] != "" {
			var ok bool
			v, ok = v.(map[string]interface{})[ss[1]]
			if !ok {
				return reflect.Invalid, "", errors.New("interface is not map[string]interface{}")
			}
		}
		if ss[2] != "" {
			i, err := strconv.Atoi(ss[2][1 : len(ss[2])-1])
			if err != nil {
				return reflect.Invalid, "", errors.New("invalid path")
			}
			v = v.([]interface{})[i]
		}
	}
	kind := reflect.ValueOf(v).Kind()
	if kind == reflect.Bool {
		return kind, fmt.Sprintf("%t", v), nil
	} else if kind == reflect.Map || kind == reflect.Slice {
		jstr, err := json.Marshal(v)
		if err != nil {
			return reflect.Invalid, "", err
		}
		return kind, fmt.Sprintf("%s", jstr), nil
	} else if kind == reflect.Float64 {
		return kind, fmt.Sprintf("%g", v), nil
	}
	return kind, fmt.Sprintf("%s", v), nil
}

func JSONFindString(v interface{}, path string) (string, error) {
	kind, outstr, err := JSONPathos(v, path)
	if err != nil {
		return "", fmt.Errorf("can't find %s in json input, %v", path, err)
	}
	if kind != reflect.String {
		return "", fmt.Errorf("string %s expected, got %v", path, kind)
	}
	return outstr, nil
}

func JSONFindLen(v interface{}, path string) (int, error) {
	kind, outstr, err := JSONPathos(v, path)
	if err != nil {
		return 0, fmt.Errorf("can't find %s in json input, %v", path, err)
	}
	if kind == reflect.Slice {
		var vv []interface{}
		if err := json.Unmarshal([]byte(outstr), &vv); err != nil {
			return 0, fmt.Errorf("can't unmarshal as slice, %v", err)
		}
		return len(vv), nil
	}
	if kind == reflect.Map {
		var vv map[string]interface{}
		if err := json.Unmarshal([]byte(outstr), &vv); err != nil {
			return 0, fmt.Errorf("can't unmarshal as map, %v", err)
		}
		return len(vv), nil
	}
	return 0, fmt.Errorf("not a map or slice, cannot get length")
}

func JSONFindSlice(v interface{}, path string) (*[]interface{}, error) {
	kind, outstr, err := JSONPathos(v, path)
	if err != nil {
		return nil, fmt.Errorf("can't find %s in json input, %v", path, err)
	}
	if kind == reflect.Slice {
		var vv []interface{}
		if err := json.Unmarshal([]byte(outstr), &vv); err != nil {
			return nil, fmt.Errorf("can't unmarshal as slice, %v", err)
		}
		return &vv, nil
	}
	return nil, fmt.Errorf("not a slice")
}

func JSONFindMap(v interface{}, path string) (*map[string]interface{}, error) {
	kind, outstr, err := JSONPathos(v, path)
	if err != nil {
		return nil, fmt.Errorf("can't find %s in json input, %v", path, err)
	}
	if kind == reflect.Map {
		var vv map[string]interface{}
		if err := json.Unmarshal([]byte(outstr), &vv); err != nil {
			return nil, fmt.Errorf("can't unmarshal as map, %v", err)
		}
		return &vv, nil
	}
	return nil, fmt.Errorf("not a map or slice, cannot get length")
}
